using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

public class ShannonFanoEncoder
{
    public Dictionary<char, string> Codes { get; private set; }
    private Dictionary<string, char> _reverseCodes;
    private List<SymbolInfo> _symbolInfos;

    public ShannonFanoEncoder()
    {
        Codes = new Dictionary<char, string>();
        _reverseCodes = new Dictionary<string, char>();
        _symbolInfos = new List<SymbolInfo>();
    }

    public void BuildCodes(List<SymbolInfo> symbolInfos)
    {
        _symbolInfos = symbolInfos;
        if (!_symbolInfos.Any()) return;

        double totalFrequency = _symbolInfos.Sum(s => s.Frequency);
        if (totalFrequency == 0 && _symbolInfos.Any())
        {
            totalFrequency = _symbolInfos.Count;
            _symbolInfos.ForEach(s => { if (s.Frequency == 0) s.Frequency = 1; });
        }

        if (totalFrequency > 0)
        {
            foreach (var si in _symbolInfos)
            {
                si.Probability = si.Frequency / totalFrequency;
            }
        }
        else if (_symbolInfos.Count == 1)
        {
            _symbolInfos.First().Probability = 1.0;
        }

        var sortedSymbols = _symbolInfos.OrderByDescending(s => s.Probability).ToList();

        Console.WriteLine("\n--- Построение кодов Шеннона-Фано ---");
        Console.WriteLine("Символы, изначально отсортированные по вероятности:");
        sortedSymbols.ForEach(s => Console.WriteLine($"  '{s.Symbol}' (Вероятность: {s.Probability:F4})"));

        AssignCodesRecursive(sortedSymbols, "");

        Codes.Clear();
        _reverseCodes.Clear();
        foreach (var si in _symbolInfos.Where(s => !string.IsNullOrEmpty(s.Code)))
        {
            if (!Codes.ContainsKey(si.Symbol))
            {
                Codes.Add(si.Symbol, si.Code);
                _reverseCodes.Add(si.Code, si.Symbol);
            }
        }

        Console.WriteLine("\n--- Итоговая таблица кодов ---");
        foreach (var codeEntry in Codes.OrderBy(kvp => kvp.Key))
        {
            Console.WriteLine($"'{codeEntry.Key}': {codeEntry.Value}");
        }
    }

    private void AssignCodesRecursive(List<SymbolInfo> symbols, string currentCode)
    {
        if (!symbols.Any()) return;

        Console.WriteLine($"\nПоследовательность: [{string.Join(", ", symbols.Select(s => $"'{s.Symbol}'({s.Probability:F4})"))}] с префиксом '{currentCode}'");

        if (symbols.Count == 1)
        {
            symbols[0].Code = currentCode;
            var originalSymbol = _symbolInfos.FirstOrDefault(s => s.Symbol == symbols[0].Symbol);
            if (originalSymbol != null) originalSymbol.Code = currentCode;
            Console.WriteLine($"  -> Листовой узел: '{symbols[0].Symbol}' получает код '{currentCode}'");
            return;
        }
        if (symbols.Count == 0)
        {
            return;
        }

        int splitIndex = FindSplitPoint(symbols);
        Console.WriteLine($"  Индекс разделения: {splitIndex} (от 0 до {splitIndex} против от {splitIndex + 1} до {symbols.Count - 1})");

        List<SymbolInfo> group1 = new List<SymbolInfo>();
        List<SymbolInfo> group2 = new List<SymbolInfo>();

        for (int i = 0; i < symbols.Count; i++)
        {
            if (i <= splitIndex)
            {
                group1.Add(symbols[i]);
            }
            else
            {
                group2.Add(symbols[i]);
            }
        }

        Console.WriteLine($"  Группа 1 (получает '0'): [{string.Join(", ", group1.Select(s => $"'{s.Symbol}'"))}]");
        AssignCodesRecursive(group1, currentCode + "0");

        Console.WriteLine($"  Группа 2 (получает '1'): [{string.Join(", ", group2.Select(s => $"'{s.Symbol}'"))}]");
        AssignCodesRecursive(group2, currentCode + "1");
    }

    private int FindSplitPoint(List<SymbolInfo> symbols)
    {
        if (symbols.Count <= 1) return 0;

        double totalProbability = symbols.Sum(s => s.Probability);
        double minDifference = double.MaxValue;
        int bestSplitIndex = 0;

        for (int i = 0; i < symbols.Count - 1; i++)
        {
            double sumGroup1 = 0;
            for (int j = 0; j <= i; j++)
            {
                sumGroup1 += symbols[j].Probability;
            }
            double sumGroup2 = totalProbability - sumGroup1;
            double difference = Math.Abs(sumGroup1 - sumGroup2);

            if (difference < minDifference)
            {
                minDifference = difference;
                bestSplitIndex = i;
            }
        }
        return bestSplitIndex;
    }

    public string Encode(string source)
    {
        if (Codes == null || !Codes.Any())
        {
            Console.WriteLine("Ошибка: Таблица кодов не построена или пуста.");
            return null;
        }
        StringBuilder encodedSource = new StringBuilder();
        Console.WriteLine("\n--- Процесс кодирования ---");
        Console.WriteLine($"Источник: \"{source}\"");
        foreach (char c in source)
        {
            if (Codes.TryGetValue(c, out string code))
            {
                encodedSource.Append(code);
                Console.WriteLine($"  '{c}' -> {code}");
            }
            else
            {
                Console.WriteLine($"  Предупреждение: Символ '{c}' не найден в таблице кодов. Пропуск.");
            }
        }
        Console.WriteLine($"Закодировано: {encodedSource.ToString()}");
        return encodedSource.ToString();
    }

    public string Decode(string encodedSource)
    {
        if (_reverseCodes == null || !_reverseCodes.Any())
        {
            Console.WriteLine("Ошибка: Обратная таблица кодов не построена или пуста.");
            return null;
        }
        StringBuilder decodedSource = new StringBuilder();
        StringBuilder currentCode = new StringBuilder();
        Console.WriteLine("\n--- Процесс декодирования ---");
        Console.WriteLine($"Закодированный источник: {encodedSource}");

        foreach (char bit in encodedSource)
        {
            currentCode.Append(bit);
            Console.Write($"  Текущие биты: {currentCode}");
            if (_reverseCodes.TryGetValue(currentCode.ToString(), out char decodedChar))
            {
                decodedSource.Append(decodedChar);
                Console.WriteLine($" -> Совпадение! Декодировано: '{decodedChar}'");
                currentCode.Clear();
            }
            else
            {
                Console.WriteLine(" -> Пока нет совпадений.");
            }
        }
        Console.WriteLine($"Декодировано: \"{decodedSource.ToString()}\"");
        return decodedSource.ToString();
    }

    public static List<SymbolInfo> GetStaticFrequencies()
    {
        var frequencies = new List<SymbolInfo>
        {
            new SymbolInfo('а', 2671), new SymbolInfo('б', 748),  new SymbolInfo('в', 358),
            new SymbolInfo('г', 2132), new SymbolInfo('д', 1551), new SymbolInfo('е', 228),
            new SymbolInfo('ё', 52),   new SymbolInfo('ж', 424),  new SymbolInfo('з', 338),
            new SymbolInfo('и', 1585), new SymbolInfo('й', 1534), new SymbolInfo('к', 77),
            new SymbolInfo('л', 2057), new SymbolInfo('м', 488),  new SymbolInfo('н', 2477),
            new SymbolInfo('о', 1674), new SymbolInfo('ө', 990),  new SymbolInfo('п', 29),
            new SymbolInfo('р', 1419), new SymbolInfo('с', 683),  new SymbolInfo('т', 1253),
            new SymbolInfo('у', 837),  new SymbolInfo('ү', 1045), new SymbolInfo('ф', 8),
            new SymbolInfo('х', 1277), new SymbolInfo('ц', 254),  new SymbolInfo('ч', 338),
            new SymbolInfo('ш', 294),  new SymbolInfo('щ', 0),   new SymbolInfo('ъ', 0),
            new SymbolInfo('ы', 407),  new SymbolInfo('ь', 297),  new SymbolInfo('э', 1918),
            new SymbolInfo('ю', 42),   new SymbolInfo('я', 89)
        };
        return frequencies.Where(s => s.Frequency > 0).ToList();
    }

    public static List<SymbolInfo> GetDynamicFrequencies(string source)
    {
        var charCounts = new Dictionary<char, int>();
        foreach (char c in source)
        {
            if (charCounts.ContainsKey(c))
            {
                charCounts[c]++;
            }
            else
            {
                charCounts[c] = 1;
            }
        }
        return charCounts.Select(kvp => new SymbolInfo(kvp.Key, kvp.Value)).ToList();
    }
}

public class SymbolInfo
{
    public char Symbol { get; set; }
    public double Frequency { get; set; }
    public double Probability { get; set; }
    public string Code { get; set; } = "";

    public SymbolInfo(char symbol, double frequency)
    {
        Symbol = symbol;
        Frequency = frequency;
    }

    public override string ToString()
    {
        return $"'{Symbol}': Частота={Frequency:F0}, Вероятность={Probability:F4}, Код='{Code}'";
    }
}

class Program
{
    static void Main(string[] args)
    {
        string inputText = "водчиц анастасия";
        Console.WriteLine($"Оригинальный текст: \"{inputText}\" (Длина: {inputText.Length} символов)");
        Console.WriteLine("======================================================");

        inputText = inputText.Replace(" ", "");

        Console.WriteLine("Режим A: Статическая вероятность");
        Console.WriteLine("------------------------------------------------------");
        ShannonFanoEncoder staticEncoder = new ShannonFanoEncoder();
        List<SymbolInfo> staticSymbolData = ShannonFanoEncoder.GetStaticFrequencies();

        string lowerInputText = inputText.ToLower();
        Console.WriteLine($"Входящая последовательность: \"{lowerInputText}\"");
        var relevantStaticSymbols = new List<SymbolInfo>();
        var uniqueCharsInLowerInput = lowerInputText.Distinct().ToList();

        foreach (char c in uniqueCharsInLowerInput)
        {
            var staticInfo = staticSymbolData.FirstOrDefault(s => s.Symbol == c);
            if (staticInfo != null)
            {
                relevantStaticSymbols.Add(new SymbolInfo(c, staticInfo.Frequency));
            }
            else
            {
                Console.WriteLine($"Предупреждение: Символ '{c}' из входных данных не найден в статической таблице частот. Он не будет закодирован в статическом режиме.");
            }
        }

        if (relevantStaticSymbols.Any())
        {
            staticEncoder.BuildCodes(relevantStaticSymbols);
            string staticEncoded = staticEncoder.Encode(lowerInputText);
            if (staticEncoded != null)
            {
                string staticDecoded = staticEncoder.Decode(staticEncoded);
                Console.WriteLine($"\nРезультаты статического режима для \"{lowerInputText}\":");
                Console.WriteLine($"Оригинал: {lowerInputText}");
                Console.WriteLine($"Закодировано:   {staticEncoded} (Длина: {staticEncoded.Length} бит)");
                Console.WriteLine($"Декодировано:   {staticDecoded}");
                CalculateEfficiency(lowerInputText, staticEncoded, "Статический");
            }
        }
        else
        {
            Console.WriteLine("В статической таблице не найдено релевантных символов из входных данных для кодирования.");
        }

        Console.WriteLine("\n======================================================");
        Console.WriteLine("Режим B: Динамическая вероятность");
        Console.WriteLine("------------------------------------------------------");
        ShannonFanoEncoder dynamicEncoder = new ShannonFanoEncoder();
        List<SymbolInfo> dynamicSymbolData = ShannonFanoEncoder.GetDynamicFrequencies(inputText);

        dynamicEncoder.BuildCodes(dynamicSymbolData);
        string dynamicEncoded = dynamicEncoder.Encode(inputText);
        if (dynamicEncoded != null)
        {
            string dynamicDecoded = dynamicEncoder.Decode(dynamicEncoded);
            Console.WriteLine($"\nРезультаты динамического режима для \"{inputText}\":");
            Console.WriteLine($"Оригинал: {inputText}");
            Console.WriteLine($"Закодировано:  {dynamicEncoded} (Длина: {dynamicEncoded.Length} бит)");
            Console.WriteLine($"Декодировано:  {dynamicDecoded}");
            CalculateEfficiency(inputText, dynamicEncoded, "Динамический");
        }
        Console.WriteLine("\n======================================================");
    }

    static void CalculateEfficiency(string originalText, string encodedText, string modeName)
    {
        if (string.IsNullOrEmpty(originalText) || string.IsNullOrEmpty(encodedText))
        {
            Console.WriteLine($"Невозможно рассчитать эффективность для {modeName}: неверные входные данные.");
            return;
        }

        int originalBits = originalText.Length * 8;
        int encodedBits = encodedText.Length;

        Console.WriteLine($"\n--- Эффективность ({modeName}) ---");
        Console.WriteLine($"Исходный размер: {originalText.Length} симв. * 8 бит/симв. (ASCII) = {originalBits} бит");
        Console.WriteLine($"Размер закодированного:  {encodedBits} бит");

        if (originalBits > 0)
        {
            double compressionRatio = (double)encodedBits / originalBits;
            double spaceSaving = 1.0 - compressionRatio;
            Console.WriteLine($"Коэффициент сжатия (закодир./исходный): {compressionRatio:P2}");
            Console.WriteLine($"Экономия места: {spaceSaving:P2}");
        }
        else
        {
            Console.WriteLine("Исходный размер равен 0, невозможно рассчитать коэффициент.");
        }
    }
}