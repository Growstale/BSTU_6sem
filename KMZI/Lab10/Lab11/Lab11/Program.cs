using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

public class ArithmeticCoding
{
    public class SymbolInfo
    {
        public char Symbol { get; set; }
        public decimal Low { get; set; }
        public decimal High { get; set; }
        public decimal Probability { get; set; }
    }

    public ArithmeticCoding() { }


    public static Dictionary<char, decimal> GetProbabilities(string text)
    {
        var counts = new Dictionary<char, int>();
        if (string.IsNullOrEmpty(text)) return counts.ToDictionary(kvp => kvp.Key, kvp => 0m);

        foreach (char c in text)
        {
            if (counts.ContainsKey(c))
                counts[c]++;
            else
                counts[c] = 1;
        }

        var probabilities = new Dictionary<char, decimal>();
        int totalLength = text.Length;
        foreach (var pair in counts)
        {
            probabilities[pair.Key] = (decimal)pair.Value / totalLength;
        }
        return probabilities;
    }

    public static List<SymbolInfo> BuildSymbolInfoTable(Dictionary<char, decimal> charProbabilities)
    {
        var symbolInfoTable = new List<SymbolInfo>();
        decimal cumulativeLow = 0m;
        var sortedSymbols = charProbabilities.Keys.OrderBy(c => c).ToList();

        foreach (char symbol in sortedSymbols)
        {
            decimal probability = charProbabilities[symbol];
            symbolInfoTable.Add(new SymbolInfo
            {
                Symbol = symbol,
                Probability = probability,
                Low = cumulativeLow,
                High = cumulativeLow + probability
            });
            cumulativeLow += probability;
        }

        if (symbolInfoTable.Any())
        {
            var lastSymbol = symbolInfoTable.Last();
            if (lastSymbol.High != 1.0m)
            {
                if (symbolInfoTable.Count > 1)
                {
                    decimal sumOfPrevProbs = symbolInfoTable.Take(symbolInfoTable.Count - 1).Sum(s => s.Probability);
                    lastSymbol.Probability = 1.0m - sumOfPrevProbs;
                    lastSymbol.Low = symbolInfoTable.Count > 1 ? symbolInfoTable[symbolInfoTable.Count - 2].High : 0m;
                    lastSymbol.High = 1.0m;
                }
                else
                {
                    lastSymbol.Probability = 1.0m;
                    lastSymbol.Low = 0m;
                    lastSymbol.High = 1.0m;
                }
            }
        }
        return symbolInfoTable;
    }

    public decimal Encode(string textToEncode, List<SymbolInfo> symbolInfoTable, bool verbose = false)
    {
        decimal low = 0.0m;
        decimal high = 1.0m;

        if (verbose) Console.WriteLine("\n--- Шаги кодирования ---");
        if (verbose) Console.WriteLine($"Начальный интервал: Low = {low:F28}, High = {high:F28}");

        int step = 1;
        foreach (char symbol in textToEncode)
        {
            decimal currentRange = high - low;
            SymbolInfo sInfo = symbolInfoTable.FirstOrDefault(s => s.Symbol == symbol);

            if (sInfo == null) throw new ArgumentException($"Символ '{symbol}' не найден в таблице вероятностей.");

            decimal oldLow = low;
            decimal oldHigh = high;

            high = low + currentRange * sInfo.High;
            low = low + currentRange * sInfo.Low;

            if (verbose)
            {
                Console.WriteLine($"Шаг {step}: Кодируем символ '{symbol}' (L0={sInfo.Low:F6}, H0={sInfo.High:F6})");
                Console.WriteLine($"  Предыдущий интервал: Low = {oldLow:F28}, High = {oldHigh:F28}, Range = {currentRange:F28}");
                Console.WriteLine($"  Новый интервал:     Low = {low:F28}, High = {high:F28}");
            }
            step++;
        }
        if (verbose) Console.WriteLine("--- Конец кодирования ---");
        return low;
    }

    public string Decode(decimal encodedValue, int originalLength, List<SymbolInfo> symbolInfoTable, bool verbose = false)
    {
        StringBuilder decodedText = new StringBuilder();
        decimal low = 0.0m;
        decimal high = 1.0m;
        const decimal MIN_COMPARISON_PRECISION = 0.0000000000000000000000000001m;

        if (verbose) Console.WriteLine("\n--- Шаги декодирования ---");
        if (verbose) Console.WriteLine($"Начальное значение для декодирования: {encodedValue:F28}");
        if (verbose) Console.WriteLine($"Начальный интервал: Low = {low:F28}, High = {high:F28}");


        for (int i = 0; i < originalLength; i++)
        {
            decimal currentRange = high - low;

            if (verbose) Console.WriteLine($"\nШаг {i + 1}:");
            if (verbose) Console.WriteLine($"  Текущий интервал: Low = {low:F28}, High = {high:F28}, Range = {currentRange:F28}");


            if (currentRange < MIN_COMPARISON_PRECISION)
            {
                Console.WriteLine($"  Критически малый диапазон: {currentRange}. Декодирование может быть неточным.");
                SymbolInfo emergencySymbol = symbolInfoTable.OrderBy(s => Math.Abs(encodedValue - (low + currentRange * s.Low))).First();
                decodedText.Append(emergencySymbol.Symbol);
                if (verbose) Console.WriteLine($"  Аварийно выбран символ '{emergencySymbol.Symbol}' из-за малого диапазона.");
                decimal temp_high = low + currentRange * emergencySymbol.High;
                low = low + currentRange * emergencySymbol.Low;
                high = temp_high;
                continue;
            }

            decimal valueInCurrentRange = (encodedValue - low) / currentRange;
            if (verbose) Console.WriteLine($"  Масштабированное значение (encodedValue - Low) / Range = {valueInCurrentRange:F28}");


            SymbolInfo foundSymbol = null;
            foreach (var sInfo in symbolInfoTable)
            {
                if (valueInCurrentRange >= sInfo.Low && (valueInCurrentRange < sInfo.High || (Math.Abs(valueInCurrentRange - sInfo.High) < MIN_COMPARISON_PRECISION && sInfo.High == 1.0m)))
                {
                    foundSymbol = sInfo;
                    break;
                }
            }

            if (foundSymbol == null)
            {
                if (Math.Abs(valueInCurrentRange - 1.0m) < MIN_COMPARISON_PRECISION && symbolInfoTable.Last().High == 1.0m)
                {
                    foundSymbol = symbolInfoTable.Last();
                }
                else
                {
                    Console.WriteLine($"  Предупреждение: Символ не найден для масштабированного значения {valueInCurrentRange}. EncodedValue: {encodedValue}");
                    foundSymbol = symbolInfoTable.OrderBy(s => Math.Abs(valueInCurrentRange - s.Low)).First();
                    Console.WriteLine($"  Аварийно выбран ближайший символ по L0: '{foundSymbol.Symbol}'");
                }
            }

            decodedText.Append(foundSymbol.Symbol);
            if (verbose) Console.WriteLine($"  Найден символ: '{foundSymbol.Symbol}' (L0={foundSymbol.Low:F6}, H0={foundSymbol.High:F6})");


            decimal oldLow = low;
            high = low + currentRange * foundSymbol.High;
            low = oldLow + currentRange * foundSymbol.Low;

            if (verbose) Console.WriteLine($"  Обновленный интервал: Low = {low:F28}, High = {high:F28}");
        }
        if (verbose) Console.WriteLine("--- Конец декодирования ---");
        return decodedText.ToString();
    }
}

public class Lab11
{
    public static void Main(string[] args)
    {
        Console.OutputEncoding = Encoding.UTF8;
        bool enableVerboseOutput = true;

        string message1 = "мультимиллионер";
        Console.WriteLine($"--- Обработка сообщения 1: '{message1}' ---");
        Dictionary<char, decimal> probs1 = ArithmeticCoding.GetProbabilities(message1);
        List<ArithmeticCoding.SymbolInfo> symbolTable1 = ArithmeticCoding.BuildSymbolInfoTable(probs1);
        Console.WriteLine("\nМодель для сообщения 1 (L0, H0 диапазоны):");
        foreach (var sInfo in symbolTable1.OrderBy(s => s.Symbol))
        {
            Console.WriteLine($"Символ: '{sInfo.Symbol}', P: {sInfo.Probability:F6}, L0: {sInfo.Low:F28}, H0: {sInfo.High:F28}");
        }
        ArithmeticCoding coder1 = new ArithmeticCoding();
        decimal encoded1 = coder1.Encode(message1, symbolTable1, enableVerboseOutput);
        Console.WriteLine($"\nИтоговое закодированное значение для '{message1}': {encoded1:F28}");
        string decoded1 = coder1.Decode(encoded1, message1.Length, symbolTable1, enableVerboseOutput);
        Console.WriteLine($"\nИтоговое декодированное сообщение: '{decoded1}'");
        Console.WriteLine($"Сообщения совпадают: {message1 == decoded1}");
        Console.WriteLine(new string('-', 50));

        string part1_msg2 = "мультимиллионер";
        string part2_msg2 = "семенохранилище";
        string message2 = part1_msg2 + part2_msg2;
        Console.WriteLine($"\n--- Обработка сообщения 2: '{message2}' ---");
        Dictionary<char, decimal> probs2 = ArithmeticCoding.GetProbabilities(message2);
        List<ArithmeticCoding.SymbolInfo> symbolTable2 = ArithmeticCoding.BuildSymbolInfoTable(probs2);
        Console.WriteLine("\nМодель для сообщения 2 (L0, H0 диапазоны):");
        foreach (var sInfo in symbolTable2.OrderBy(s => s.Symbol))
        {
            Console.WriteLine($"Символ: '{sInfo.Symbol}', P: {sInfo.Probability:F6}, L0: {sInfo.Low:F28}, H0: {sInfo.High:F28}");
        }
        ArithmeticCoding coder2 = new ArithmeticCoding();
        decimal encoded2 = coder2.Encode(message2, symbolTable2, enableVerboseOutput);
        Console.WriteLine($"\nИтоговое закодированное значение для '{message2}': {encoded2:F28}");
        string decoded2 = coder2.Decode(encoded2, message2.Length, symbolTable2, enableVerboseOutput);
        Console.WriteLine($"\nИтоговое декодированное сообщение: '{decoded2}'");
        Console.WriteLine($"Сообщения совпадают: {message2 == decoded2}");
        Console.WriteLine(new string('-', 50));

        Console.WriteLine("\nНажмите любую клавишу для выхода...");
        Console.ReadKey();
    }
}