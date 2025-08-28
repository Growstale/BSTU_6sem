using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Diagnostics;
using System.IO;

public class LZ77Coder
{
    private int dictionarySizeN1;
    private int lookaheadBufferSizeN2;
    private bool enableVerboseOutput;

    public LZ77Coder(int n1, int n2, bool verbose = false)
    {
        if (n1 <= 0 || n2 <= 0)
        {
            throw new ArgumentException("Размеры словаря и буфера упреждения должны быть положительными.");
        }
        dictionarySizeN1 = n1;
        lookaheadBufferSizeN2 = n2;
        enableVerboseOutput = verbose;
    }

    public struct Triplet
    {
        public int P { get; }
        public int Q { get; }
        public char S { get; }

        public Triplet(int p, int q, char s)
        {
            P = p;
            Q = q;
            S = s;
        }

        public override string ToString()
        {
            return $"({P},{Q},'{S.ToString().Replace("\0", "\\0")}')";
        }
    }

    public List<Triplet> Encode(string inputText)
    {
        List<Triplet> encodedTriplets = new List<Triplet>();
        StringBuilder dictionary = new StringBuilder(new string('\0', dictionarySizeN1));
        int currentPositionInInput = 0;
        int step = 1;

        if (enableVerboseOutput)
        {
            Console.WriteLine("\n--- Начало Кодирования ---");
            Console.WriteLine($"Размер словаря (n1): {dictionarySizeN1}, Размер буфера упреждения (n2): {lookaheadBufferSizeN2}");
            Console.WriteLine($"Начальный словарь: \"{dictionary.ToString().Replace("\0", ".")}\"");
        }

        while (currentPositionInInput < inputText.Length)
        {
            if (enableVerboseOutput)
            {
                Console.WriteLine($"\nШаг {step}:");
                Console.WriteLine($"  Словарь: \"{dictionary.ToString().Replace("\0", ".")}\"");
            }

            int lookaheadActualSize = Math.Min(lookaheadBufferSizeN2, inputText.Length - currentPositionInInput);
            if (lookaheadActualSize == 0) break;

            string lookaheadBuffer = inputText.Substring(currentPositionInInput, lookaheadActualSize);

            if (enableVerboseOutput)
            {
                Console.WriteLine($"  Буфер упреждения ({lookaheadActualSize}): \"{lookaheadBuffer}\"");
            }

            int bestMatchOffset = 0;
            int bestMatchLength = 0;
            string foundMatchString = "";

            for (int q_try = lookaheadActualSize; q_try >= 1; q_try--)
            {
                string prefixToMatch = lookaheadBuffer.Substring(0, q_try);
                for (int p_try = 0; p_try <= dictionarySizeN1 - q_try; p_try++)
                {
                    string dictSub = dictionary.ToString().Substring(p_try, q_try);
                    if (dictSub == prefixToMatch)
                    {
                        bestMatchOffset = p_try;
                        bestMatchLength = q_try;
                        foundMatchString = dictSub;
                        goto FoundBestMatchForCurrentPass;
                    }
                }
            }
        FoundBestMatchForCurrentPass:;


            char nextSymbolS;
            if (bestMatchLength == 0)
            {
                nextSymbolS = lookaheadBuffer[0];
                if (enableVerboseOutput) Console.WriteLine($"  Нет совпадения. s = '{nextSymbolS}'");
            }
            else
            {
                if (currentPositionInInput + bestMatchLength < inputText.Length)
                {
                    nextSymbolS = inputText[currentPositionInInput + bestMatchLength];
                }
                else
                {
                    nextSymbolS = '\0';
                }
                if (enableVerboseOutput) Console.WriteLine($"  Найдено совпадение: \"{foundMatchString}\" (p={bestMatchOffset}, q={bestMatchLength}). s = '{nextSymbolS.ToString().Replace("\0", "\\0")}'");
            }

            encodedTriplets.Add(new Triplet(bestMatchOffset, bestMatchLength, nextSymbolS));
            if (enableVerboseOutput) Console.WriteLine($"  Выведена триада: ({bestMatchOffset},{bestMatchLength},'{nextSymbolS.ToString().Replace("\0", "\\0")}')");

            int shiftAmount = bestMatchLength + 1;
            string stringToShiftIntoDictionary;

            if (currentPositionInInput + shiftAmount <= inputText.Length)
            {
                stringToShiftIntoDictionary = inputText.Substring(currentPositionInInput, shiftAmount);
            }
            else
            {
                stringToShiftIntoDictionary = inputText.Substring(currentPositionInInput, inputText.Length - currentPositionInInput);
            }

            if (nextSymbolS == '\0' && stringToShiftIntoDictionary.Length > bestMatchLength && bestMatchLength > 0)
            {
                stringToShiftIntoDictionary = stringToShiftIntoDictionary.Substring(0, bestMatchLength);
            }
            else if (nextSymbolS == '\0' && bestMatchLength == 0 && stringToShiftIntoDictionary.Length > 0)
            {
            }


            if (stringToShiftIntoDictionary.Length > 0)
            {
                if (dictionary.Length + stringToShiftIntoDictionary.Length > dictionarySizeN1)
                {
                    dictionary.Remove(0, Math.Min(dictionary.Length, (dictionary.Length + stringToShiftIntoDictionary.Length) - dictionarySizeN1));
                }
                dictionary.Append(stringToShiftIntoDictionary);
                if (dictionary.Length > dictionarySizeN1)
                {
                    dictionary.Remove(0, dictionary.Length - dictionarySizeN1);
                }
            }

            currentPositionInInput += shiftAmount;
            step++;

            if (nextSymbolS == '\0')
            {
                if (enableVerboseOutput) Console.WriteLine("  Достигнут конец входной строки с символом конца потока в триаде.");
                break;
            }
        }
        if (enableVerboseOutput) Console.WriteLine("--- Конец Кодирования ---");
        return encodedTriplets;
    }

    public string Decode(List<Triplet> encodedTriplets)
    {
        StringBuilder decodedText = new StringBuilder();
        StringBuilder dictionary = new StringBuilder(new string('\0', dictionarySizeN1));
        int step = 1;

        if (enableVerboseOutput)
        {
            Console.WriteLine("\n--- Начало Декодирования ---");
            Console.WriteLine($"Размер словаря (n1): {dictionarySizeN1}");
            Console.WriteLine($"Начальный словарь: \"{dictionary.ToString().Replace("\0", ".")}\"");
        }

        foreach (Triplet triplet in encodedTriplets)
        {
            if (enableVerboseOutput)
            {
                Console.WriteLine($"\nШаг {step}:");
                Console.WriteLine($"  Словарь: \"{dictionary.ToString().Replace("\0", ".")}\"");
                Console.WriteLine($"  Принята триада: {triplet}");
            }

            int p = triplet.P;
            int q = triplet.Q;
            char s = triplet.S;
            string partToAppendToTextAndShiftToDictionary = "";

            if (q > 0)
            {
                if (p + q > dictionary.Length)
                {
                    if (enableVerboseOutput) Console.WriteLine($"  ВНИМАНИЕ: Попытка чтения за пределами словаря (p={p}, q={q}, dict.len={dictionary.Length}). Это может быть нормально для начала, если словарь еще пуст.");
                }

                string matchedString = dictionary.ToString().Substring(p, q);
                decodedText.Append(matchedString);
                partToAppendToTextAndShiftToDictionary += matchedString;
                if (enableVerboseOutput) Console.WriteLine($"  Скопировано из словаря: \"{matchedString}\"");
            }
            else
            {
                if (enableVerboseOutput) Console.WriteLine($"  Нет совпадения для копирования (q=0).");
            }

            if (s != '\0')
            {
                decodedText.Append(s);
                partToAppendToTextAndShiftToDictionary += s;
                if (enableVerboseOutput) Console.WriteLine($"  Добавлен символ s: '{s}'");
            }
            else
            {
                if (enableVerboseOutput) Console.WriteLine($"  Символ s - маркер конца потока '\\0', не добавляется в текст и не сдвигается в словарь.");
            }

            if (partToAppendToTextAndShiftToDictionary.Length > 0)
            {
                if (dictionary.Length + partToAppendToTextAndShiftToDictionary.Length > dictionarySizeN1)
                {
                    dictionary.Remove(0, Math.Min(dictionary.Length, (dictionary.Length + partToAppendToTextAndShiftToDictionary.Length) - dictionarySizeN1));
                }
                dictionary.Append(partToAppendToTextAndShiftToDictionary);
                if (dictionary.Length > dictionarySizeN1)
                {
                    dictionary.Remove(0, dictionary.Length - dictionarySizeN1);
                }
            }

            if (enableVerboseOutput) Console.WriteLine($"  Восстановленный текст на данный момент (часть): ...{decodedText.ToString().Substring(Math.Max(0, decodedText.Length - 30))}");

            step++;
            if (s == '\0')
            {
                if (enableVerboseOutput) Console.WriteLine("  Достигнут символ конца потока в триаде. Декодирование завершено.");
                break;
            }
        }
        if (enableVerboseOutput) Console.WriteLine("--- Конец Декодирования ---");
        return decodedText.ToString();
    }
}

class Program
{
    static readonly (int n1, int n2)[] windowSizeCombinations = new (int, int)[]
    {
        (15, 13),
        (32, 16),
        (64, 32),
        (128, 64),
        (256, 128),
        (512, 256),
        (1024, 512),
        (60, 40)
    };

    struct TestResult
    {
        public int N1;
        public int N2;
        public long EncodeTimeMs;
        public long DecodeTimeMs;
        public int TripletCount;
        public int OriginalLength;
        public double CompressionRatio;
        public bool IsCorrect;
    }


    static void Main(string[] args)
    {
        Console.OutputEncoding = System.Text.Encoding.UTF8;
        Console.InputEncoding = System.Text.Encoding.UTF8;

        string inputText = "";
        string filePath = "D:\\6sem_BSTU\\KMZI\\Lab10\\Lab10\\Lab10\\inputText.txt";

        try
        {
            inputText = File.ReadAllText(filePath, System.Text.Encoding.UTF8);
            Console.WriteLine($"Загружен текст из файла {filePath}, длина: {inputText.Length} символов.\n");
        }
        catch (FileNotFoundException)
        {
            Console.WriteLine($"Файл {filePath} не найден. Используется короткая тестовая строка.");
            inputText = "abracadabraabracadabraabracadabraabracadabra";
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Ошибка при чтении файла: {ex.Message}");
            return;
        }

        bool enableVerbose = false;
        if (inputText.Length < 200)
        {
            Console.Write("Включить подробный вывод шагов? (y/n, рекомендуется для коротких строк): ");
            if (Console.ReadKey().KeyChar.ToString().ToLower() == "y")
            {
                enableVerbose = true;
            }
            Console.WriteLine();
        }
        else
        {
            Console.WriteLine("Подробный вывод шагов кодирования/декодирования отключен для длинного текста.");
        }

        Console.WriteLine($"\n--- Исходные данные ---");
        Console.WriteLine($"Последовательность ДО сжатия ({inputText.Length} символов):");
        const int MAX_DISPLAY_LENGTH_TEXT = 200;
        if (enableVerbose || inputText.Length <= MAX_DISPLAY_LENGTH_TEXT)
        {
            Console.WriteLine($"\"{inputText}\"");
        }
        else
        {
            Console.WriteLine($"\"{inputText.Substring(0, MAX_DISPLAY_LENGTH_TEXT / 2)}...{inputText.Substring(inputText.Length - MAX_DISPLAY_LENGTH_TEXT / 2)}\" (сокращено, всего {inputText.Length})");
        }
        Console.WriteLine("------------------------\n");

        List<TestResult> results = new List<TestResult>();

        Console.WriteLine("\n--- Запуск тестов с разными размерами окон ---");

        foreach (var (n1, n2) in windowSizeCombinations)
        {
            Console.WriteLine($"\nТестирование с n1={n1}, n2={n2}");
            LZ77Coder coder = new LZ77Coder(n1, n2, enableVerbose);

            Stopwatch stopwatchEncode = Stopwatch.StartNew();
            List<LZ77Coder.Triplet> encodedData = coder.Encode(inputText);
            stopwatchEncode.Stop();

            Console.WriteLine($"  Последовательность ПОСЛЕ сжатия ({encodedData.Count} триад):");
            const int MAX_DISPLAY_TRIPLETS = 30;
            if (enableVerbose || encodedData.Count <= MAX_DISPLAY_TRIPLETS)
            {
                Console.Write("    ");
                foreach (var triplet in encodedData)
                {
                    Console.Write(triplet + " ");
                }
                Console.WriteLine();
            }
            else
            {
                Console.Write("    ");
                for (int i = 0; i < MAX_DISPLAY_TRIPLETS / 2; i++) Console.Write(encodedData[i] + " ");
                Console.Write("... ");
                int startIndexLastPart = Math.Max(MAX_DISPLAY_TRIPLETS / 2, encodedData.Count - (MAX_DISPLAY_TRIPLETS / 2));
                for (int i = startIndexLastPart; i < encodedData.Count; i++) Console.Write(encodedData[i] + " ");
                Console.WriteLine($" (сокращено, всего {encodedData.Count} триад)");
            }

            Stopwatch stopwatchDecode = Stopwatch.StartNew();
            string decodedText = coder.Decode(encodedData);
            stopwatchDecode.Stop();

            if (enableVerbose)
            {
                Console.WriteLine($"  Восстановленная последовательность ({decodedText.Length} символов):");
                if (decodedText.Length <= MAX_DISPLAY_LENGTH_TEXT)
                {
                    Console.WriteLine($"    \"{decodedText}\"");
                }
                else
                {
                    Console.WriteLine($"    \"{decodedText.Substring(0, MAX_DISPLAY_LENGTH_TEXT / 2)}...{decodedText.Substring(decodedText.Length - MAX_DISPLAY_LENGTH_TEXT / 2)}\" (сокращено, всего {decodedText.Length})");
                }
            }

            bool isCorrect = (inputText == decodedText);
            if (!isCorrect && !enableVerbose)
            {
                Console.WriteLine("  ОШИБКА: Исходный и декодированный тексты НЕ совпадают!");
                if (inputText.Length < 100 && decodedText.Length < 100)
                {
                    Console.WriteLine($"    Оригинал:   \"{inputText}\"");
                    Console.WriteLine($"    Декодир.: \"{decodedText}\"");
                }
            }


            int bitsPerSymbolInText = 8;
            long originalSizeBits = (long)inputText.Length * bitsPerSymbolInText;
            long originalSizeBytes = originalSizeBits / 8;


            int bitsForP = (n1 > 1) ? (int)Math.Ceiling(Math.Log2(n1)) : 0;
            int bitsForQ = (n2 > 0) ? (int)Math.Ceiling(Math.Log2(n2 + 1)) : 0;
            int bitsPerTripletTheoretical = bitsForP + bitsForQ + bitsPerSymbolInText;

            long compressedSizeBits = (long)encodedData.Count * bitsPerTripletTheoretical;
            long compressedSizeBytes = compressedSizeBits / 8;
            if (compressedSizeBits % 8 != 0) compressedSizeBytes++;

            double compressionRatio = 0;
            if (originalSizeBytes > 0)
            {
                compressionRatio = (1.0 - (double)compressedSizeBytes / originalSizeBytes) * 100.0;
            }


            results.Add(new TestResult
            {
                N1 = n1,
                N2 = n2,
                EncodeTimeMs = stopwatchEncode.ElapsedMilliseconds,
                DecodeTimeMs = stopwatchDecode.ElapsedMilliseconds,
                TripletCount = encodedData.Count,
                OriginalLength = inputText.Length,
                CompressionRatio = compressionRatio,
                IsCorrect = isCorrect
            });

            Console.WriteLine($"  Кодирование: {stopwatchEncode.ElapsedMilliseconds} мс, Триад: {encodedData.Count}");
            Console.WriteLine($"  Декодирование: {stopwatchDecode.ElapsedMilliseconds} мс, Корректно: {isCorrect}");
            Console.WriteLine($"  Примерный размер до: {originalSizeBytes} байт, после: {compressedSizeBytes} байт (при {bitsPerTripletTheoretical} бит/триада)");
            Console.WriteLine($"  Примерный коэф. сжатия: {compressionRatio:F2}%");
            if (!enableVerbose && !isCorrect)
            {
                Console.WriteLine("  !!! ОБНАРУЖЕНА ОШИБКА ДЕКОДИРОВАНИЯ !!!");
            }
        }

        Console.WriteLine("\n\n--- Итоговая таблица результатов ---");
        Console.WriteLine("-----------------------------------------------------------------------------------------------------------------");
        Console.WriteLine("|  n1  |  n2  | Кодирование (мс) | Декодирование (мс) | Кол-во триад | Коэф. сжатия (%) | Корректно |");
        Console.WriteLine("-----------------------------------------------------------------------------------------------------------------");
        foreach (var res in results.OrderBy(r => r.N1).ThenBy(r => r.N2))
        {
            Console.WriteLine($"| {res.N1,4} | {res.N2,4} | {res.EncodeTimeMs,16} | {res.DecodeTimeMs,18} | {res.TripletCount,12} | {res.CompressionRatio,16:F2} | {(res.IsCorrect ? "Да" : "НЕТ"),-9} |");
        }
        Console.WriteLine("-----------------------------------------------------------------------------------------------------------------");


        Console.WriteLine("\nНажмите любую клавишу для выхода...");
        Console.ReadKey();
    }
}