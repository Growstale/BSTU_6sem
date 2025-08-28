using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

public class BWTResult
{
    public string TransformedText { get; set; }
    public int OriginalIndex { get; set; }
}

public class BurrowsWheelerTransform
{
    public static BWTResult Encode(string text)
    {
        Console.WriteLine($"--- Запуск прямого преобразования BWT для строки: '{text}' (сдвиги ВПРАВО) ---");

        if (string.IsNullOrEmpty(text))
        {
            Console.WriteLine("Входная строка пуста или null.");
            return new BWTResult { TransformedText = "", OriginalIndex = -1 };
        }

        int n = text.Length;
        List<string> rotations = new List<string>(n);
        string currentRotation = text;

        Console.WriteLine("\n1. Генерация циклических сдвигов (ротаций) ВПРАВО:");
        for (int i = 0; i < n; i++)
        {
            rotations.Add(currentRotation);
            Console.WriteLine($"   Сдвиг {i}: {currentRotation}");

            if (n > 0)
            {
                char lastChar = currentRotation[n - 1];
                string restOfString = currentRotation.Substring(0, n - 1);
                currentRotation = lastChar + restOfString;
            }
        }

        Console.WriteLine("\n2. Лексикографическая сортировка сдвигов:");
        rotations.Sort(StringComparer.Ordinal);

        Console.WriteLine("   Отсортированная матрица:");
        for (int i = 0; i < n; i++)
        {
            Console.WriteLine($"   [{i.ToString().PadLeft(2)}]: {rotations[i]}");
        }

        Console.WriteLine("\n3. Извлечение последнего столбца (L):");
        StringBuilder transformedTextBuilder = new StringBuilder(n);
        for (int i = 0; i < n; i++)
        {
            transformedTextBuilder.Append(rotations[i][n - 1]);
        }
        string transformedText = transformedTextBuilder.ToString();
        Console.WriteLine($"   Последний столбец (L): '{transformedText}'");

        Console.WriteLine("\n4. Поиск индекса исходной строки (I):");
        int originalIndex = -1;
        for (int i = 0; i < n; i++)
        {
            if (rotations[i].Equals(text, StringComparison.Ordinal))
            {
                originalIndex = i;
                break;
            }
        }

        if (originalIndex != -1)
        {
            Console.WriteLine($"   Исходная строка '{text}' найдена в отсортированной матрице по индексу: {originalIndex} (это {originalIndex + 1}-я строка)");
        }
        else
        {
            Console.WriteLine($"   Ошибка: Исходная строка '{text}' НЕ найдена в отсортированной матрице!");
        }


        Console.WriteLine("--- Прямое преобразование BWT завершено ---");
        return new BWTResult { TransformedText = transformedText, OriginalIndex = originalIndex };
    }

    public static string DecodeMatrixMethod(string transformedText, int originalIndex)
    {
        Console.WriteLine($"\n--- Запуск ОБРАТНОГО преобразования BWT (метод матрицы) для L='{transformedText}', I={originalIndex} ({originalIndex + 1}-я строка) ---");

        if (string.IsNullOrEmpty(transformedText) || originalIndex < 0)
        {
            Console.WriteLine("Некорректные входные данные для декодирования.");
            return "";
        }

        int n = transformedText.Length;
        Console.WriteLine($"Длина строки L: {n}");

        List<string> matrixRows = new List<string>(n);
        for (int i = 0; i < n; ++i)
        {
            matrixRows.Add("");
        }

        Console.WriteLine("\nНачинаем построение матрицы справа налево:");

        for (int j = 0; j < n; j++)
        {
            Console.WriteLine($"\nИтерация {j + 1} (построение {n - j}-го столбца):");

            Console.WriteLine("   1. Добавление символов L в начало строк:");
            List<string> tempRows = new List<string>(n);
            for (int i = 0; i < n; i++)
            {
                string newRow = transformedText[i] + matrixRows[i];
                tempRows.Add(newRow);
                Console.WriteLine($"      '{transformedText[i]}' + \"{matrixRows[i]}\" -> \"{newRow}\"");
            }
            matrixRows = tempRows;


            Console.WriteLine("\n   2. Сортировка строк:");
            matrixRows.Sort(StringComparer.Ordinal);

            Console.WriteLine("   Текущее состояние матрицы (строки отсортированы):");
            for (int i = 0; i < n; i++)
            {
                if (matrixRows[i].Length > 0)
                {
                    Console.Write($"     [{i.ToString().PadLeft(2)}]: '{matrixRows[i][0]}' ");
                    if (matrixRows[i].Length > 1)
                    {
                        Console.WriteLine($"| \"{matrixRows[i].Substring(1)}\"");
                    }
                    else
                    {
                        Console.WriteLine();
                    }
                }
                else
                {
                    Console.WriteLine($"     [{i.ToString().PadLeft(2)}]: \"\"");
                }
            }
        }

        Console.WriteLine("\n--- Построение матрицы завершено ---");

        if (originalIndex >= 0 && originalIndex < n)
        {
            string decodedText = matrixRows[originalIndex];
            Console.WriteLine($"\nИзвлекаем строку по индексу I={originalIndex} ({originalIndex + 1}-я строка): '{decodedText}'");
            Console.WriteLine("--- Обратное преобразование BWT завершено ---");
            return decodedText;
        }
        else
        {
            Console.WriteLine($"\nОшибка: Некорректный индекс {originalIndex}.");
            Console.WriteLine("--- Обратное преобразование BWT завершено с ошибкой ---");
            return "";
        }
    }
}

class Program
{
    static void Main(string[] args)
    {
        string inputText = "полка";
        Console.WriteLine($"Входной текст: {inputText}\n");

        Stopwatch encodeStopwatch = new Stopwatch();
        encodeStopwatch.Start();
        BWTResult encodedResult = BurrowsWheelerTransform.Encode(inputText);
        encodeStopwatch.Stop();
        TimeSpan encodeElapsedTime = encodeStopwatch.Elapsed;

        Console.WriteLine("\nРезультат кодирования:");
        Console.WriteLine($"  Преобразованный текст (L): {encodedResult.TransformedText}");
        if (encodedResult.OriginalIndex != -1)
        {
            Console.WriteLine($"  Индекс исходной строки (I): {encodedResult.OriginalIndex + 1}");
        }
        else
        {
            Console.WriteLine($"  Индекс исходной строки (I): Ошибка поиска");
        }
        Console.WriteLine($"\n>>> Время выполнения ПРЯМОГО преобразования: {encodeElapsedTime.TotalMilliseconds:F4} мс ({encodeElapsedTime.Ticks} тиков)");
        Console.WriteLine("-----------------------------------\n");

        TimeSpan decodeElapsedTime = TimeSpan.Zero;

        if (encodedResult.OriginalIndex != -1)
        {
            Stopwatch decodeStopwatch = new Stopwatch();
            decodeStopwatch.Start();
            string decodedText = BurrowsWheelerTransform.DecodeMatrixMethod(encodedResult.TransformedText, encodedResult.OriginalIndex);
            decodeStopwatch.Stop();
            decodeElapsedTime = decodeStopwatch.Elapsed;

            Console.WriteLine("\nРезультат декодирования:");
            Console.WriteLine($"  Восстановленный текст: {decodedText}");
            Console.WriteLine($"\n>>> Время выполнения ОБРАТНОГО преобразования (матричный метод): {decodeElapsedTime.TotalMilliseconds:F4} мс ({decodeElapsedTime.Ticks} тиков)");
            Console.WriteLine("\n-----------------------------------");

            Console.WriteLine($"\nПроверка: Исходный текст {(inputText == decodedText ? "совпадает с" : "НЕ СОВПАДАЕТ с")} восстановленным.");
            Console.WriteLine($"\nСравнительный анализ длительности (слово {inputText} k = {inputText.Length}):");
            Console.WriteLine($"  Прямое преобразование:      {encodeElapsedTime.TotalMilliseconds:F4} мс");
            Console.WriteLine($"  Обратное преобразование:    {decodeElapsedTime.TotalMilliseconds:F4} мс");
            if (encodeElapsedTime > decodeElapsedTime)
            {
                Console.WriteLine("  Прямое преобразование заняло больше времени.");
            }
            else if (decodeElapsedTime > encodeElapsedTime)
            {
                Console.WriteLine("  Обратное преобразование заняло больше времени.");
            }
            else
            {
                Console.WriteLine("  Время выполнения примерно одинаково.");
            }

        }
        else
        {
            Console.WriteLine("\nДекодирование не выполнено из-за ошибки при кодировании.");
        }


        Console.WriteLine("\nНажмите любую клавишу для выхода...");
        Console.ReadKey();
    }
}