using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

class Program
{
    const string STEP_PREFIX = "   -> ";
    const string DEBUG_PREFIX = "      - ";
    const string OK_STATUS = "[ OK ]";
    const string ERROR_STATUS = "[ ERROR ]";
    const string WARN_STATUS = "[ WARN ]";

    static int[,] GenerateGeneratorMatrixNonCanonical(int k, int n, int[] g)
    {
        int r = g.Length - 1;
        if (n != k + r) throw new ArgumentException("Несоответствие размеров n, k, r");
        int[,] G_noncanonical = new int[k, n];
        for (int i = 0; i < k; i++)
        {
            for (int gi = 0; gi < g.Length; gi++)
            {
                int colIndex = i + gi;
                if (colIndex < n)
                {
                    G_noncanonical[i, colIndex] = g[gi];
                }
            }
        }
        return G_noncanonical;
    }

    static void ToCanonicalForm(int[,] G, int k, int n)
    {
        Console.WriteLine(STEP_PREFIX + "Выполнение приведения к каноническому виду [Ik | P]...");
        bool possible = true;
        Dictionary<int, List<int>> rowModifications = new Dictionary<int, List<int>>();

        for (int i = 0; i < k; i++)
        {
            int pivotRow = i;
            while (pivotRow < k && G[pivotRow, i] == 0) pivotRow++;

            if (pivotRow == k)
            {
                Console.WriteLine($"{DEBUG_PREFIX}{WARN_STATUS} Столбец {i}: Не найден опорный '1'. Пропуск.");
                possible = false;
                continue;
            }

            if (pivotRow != i)
            {
                Console.WriteLine($"{DEBUG_PREFIX}Обмен:    Строка {i} <-> Строка {pivotRow}");
                List<int> history_i = rowModifications.ContainsKey(i) ? rowModifications[i] : null;
                List<int> history_pivot = rowModifications.ContainsKey(pivotRow) ? rowModifications[pivotRow] : null;
                if (history_i != null) rowModifications[pivotRow] = history_i; else rowModifications.Remove(pivotRow);
                if (history_pivot != null) rowModifications[i] = history_pivot; else rowModifications.Remove(i);
                for (int t = 0; t < n; t++) { int temp = G[i, t]; G[i, t] = G[pivotRow, t]; G[pivotRow, t] = temp; }
            }

            for (int j = i + 1; j < k; j++)
            {
                if (G[j, i] == 1)
                {
                    if (!rowModifications.ContainsKey(j)) rowModifications[j] = new List<int>();
                    rowModifications[j].Add(i);
                    for (int t = i; t < n; t++) G[j, t] ^= G[i, t];
                }
            }
        }

        if (!possible)
        {
            Console.WriteLine($"{DEBUG_PREFIX}{ERROR_STATUS} Прямой ход не завершен из-за отсутствия опорных элементов.");
        }

        for (int i = k - 1; i >= 0; i--)
        {
            if (G[i, i] == 0)
            {
                if (possible) Console.WriteLine($"{DEBUG_PREFIX}{WARN_STATUS} Столбец {i}: Опорный элемент G[{i},{i}] == 0. Пропуск.");
                continue;
            }

            for (int j = 0; j < i; j++)
            {
                if (G[j, i] == 1)
                {
                    if (!rowModifications.ContainsKey(j)) rowModifications[j] = new List<int>();
                    rowModifications[j].Add(i);
                    for (int t = i; t < n; t++) G[j, t] ^= G[i, t];
                }
            }
        }

        Console.WriteLine(DEBUG_PREFIX + "Итоговые операции сложения для строк:");
        bool modificationsFound = false;
        foreach (int targetRow in rowModifications.Keys.OrderBy(key => key))
        {
            List<int> sourceRows = rowModifications[targetRow];
            if (sourceRows != null && sourceRows.Count > 0)
            {
                sourceRows.Sort();
                Console.WriteLine($"{DEBUG_PREFIX}Строка {targetRow,-2} += Строка {string.Join(" + Строка ", sourceRows)}");
                modificationsFound = true;
            }
        }
        if (!modificationsFound)
        {
            Console.WriteLine(DEBUG_PREFIX + "(Операций сложения строк не зафиксировано)");
        }

        bool diagonalOk = true;
        for (int idx = 0; idx < k; ++idx) if (G[idx, idx] == 0) diagonalOk = false;

        if (possible && diagonalOk)
            Console.WriteLine(STEP_PREFIX + "Приведение к канонической форме завершено.");
        else
            Console.WriteLine($"{ERROR_STATUS} Приведение к канонической форме не удалось полностью завершить.");
    }

    static int[,] GenerateHMatrix(int[,] G_canonical, int k, int n)
    {
        int r = n - k;
        int[,] H = new int[r, n];
        for (int i = 0; i < r; i++)
        {
            for (int j = 0; j < k; j++)
                H[i, j] = G_canonical[j, k + i];
            H[i, k + i] = 1;
        }
        return H;
    }

    static int[] MultiplyVectorByGeneratorMatrix(int[] vector, int[,] G_canonical)
    {
        int k = G_canonical.GetLength(0);
        int n = G_canonical.GetLength(1);
        if (vector.Length != k) throw new ArgumentException("Длина вектора должна быть равна k");
        int[] result = new int[n];
        for (int i = 0; i < k; i++)
            if (vector[i] == 1)
                for (int j = 0; j < n; j++)
                    result[j] ^= G_canonical[i, j];
        return result;
    }

    static int[] ComputeSyndrome(int[,] H, int[] received)
    {
        int r = H.GetLength(0);
        int n = H.GetLength(1);
        if (received.Length != n) throw new ArgumentException("Длина принятого слова должна быть равна n");
        int[] syndrome = new int[r];
        for (int i = 0; i < r; i++)
        {
            int sum = 0;
            for (int j = 0; j < n; j++)
                sum ^= H[i, j] * received[j];
            syndrome[i] = sum;
        }
        return syndrome;
    }

    static int FindErrorPosition(int[] syndrome, int[,] H)
    {
        int r = H.GetLength(0);
        int n = H.GetLength(1);
        if (syndrome.All(bit => bit == 0)) return -1;
        for (int j = 0; j < n; j++)
        {
            bool match = true;
            for (int i = 0; i < r; i++)
                if (H[i, j] != syndrome[i]) { match = false; break; }
            if (match) return j;
        }
        return -2;
    }

    static int[] CorrectSingleError(int[] received, int errorPosition)
    {
        int[] corrected = (int[])received.Clone();
        if (errorPosition >= 0 && errorPosition < corrected.Length)
            corrected[errorPosition] ^= 1;
        return corrected;
    }

    static int[] IntroduceErrors(int[] codeword, int errorCount, Random rand, out List<int> flippedPositions)
    {
        int n = codeword.Length;
        int[] corrupted = (int[])codeword.Clone();
        flippedPositions = new List<int>();
        if (errorCount <= 0 || errorCount > n) return corrupted;
        while (flippedPositions.Count < errorCount)
        {
            int pos = rand.Next(n);
            if (!flippedPositions.Contains(pos))
            {
                corrupted[pos] ^= 1;
                flippedPositions.Add(pos);
            }
        }
        flippedPositions.Sort();
        return corrupted;
    }

    static void PrintMatrixStyled(string title, int[,] matrix, int separatorPos = -1)
    {
        Console.WriteLine($"\n{title} ({matrix.GetLength(0)}x{matrix.GetLength(1)}):");
        int rows = matrix.GetLength(0);
        int cols = matrix.GetLength(1);
        if (rows == 0 || cols == 0) { Console.WriteLine("  (Пустая матрица)"); return; }

        string separatorLine = "  +";
        separatorLine += (separatorPos > 0 && separatorPos < cols)
            ? new string('-', separatorPos * 2) + "+" + new string('-', (cols - separatorPos) * 2 - 1) + "+"
            : new string('-', cols * 2) + "+";
        Console.WriteLine(separatorLine);

        for (int i = 0; i < rows; i++)
        {
            Console.Write("  | ");
            for (int j = 0; j < cols; j++)
            {
                Console.Write(matrix[i, j] + " ");
                if (j == separatorPos - 1) Console.Write("| ");
            }
            Console.WriteLine("|");
        }
        Console.WriteLine(separatorLine);
    }

    static string FormatVector(int[] vector, List<int> highlightPositions = null)
    {
        if (vector == null) return "[null]";
        // Используем HashSet для быстрой проверки принадлежности
        HashSet<int> highlights = highlightPositions != null ? new HashSet<int>(highlightPositions) : new HashSet<int>();
        StringBuilder sb = new StringBuilder("[");
        for (int i = 0; i < vector.Length; ++i)
        {
            bool highlight = highlights.Contains(i); // Проверяем наличие в списке/множестве
            if (highlight) sb.Append("\u001b[31;1m");
            sb.Append(vector[i]);
            if (highlight) sb.Append("\u001b[0m");
            if (i < vector.Length - 1) sb.Append(" ");
        }
        sb.Append("]");
        return sb.ToString();
    }

    static void PrintStatus(string label, bool success)
    {
        Console.Write(label);
        Console.ForegroundColor = success ? ConsoleColor.Green : ConsoleColor.Red;
        Console.Write(success ? OK_STATUS : ERROR_STATUS);
        Console.ResetColor();
        Console.WriteLine();
    }

    static void AnalyzeWithErrorCount(int[] codeword, int[,] H, int errorCount, int n, Random rand)
    {
        Console.WriteLine($"\n{new string('=', 15)} АНАЛИЗ С {errorCount} ОШИБКАМИ {new string('=', 15)}");

        List<int> errorPositionsIntroduced;
        int[] received = IntroduceErrors(codeword, errorCount, rand, out errorPositionsIntroduced);

        Console.WriteLine($"Исходное слово Xn: {FormatVector(codeword)}");
        if (errorCount > 0)
        {
            Console.WriteLine($"Внесены ошибки на позициях (0-индекс): {string.Join(", ", errorPositionsIntroduced)}");
            // ВЫЗОВ С ПЕРЕДАЧЕЙ ВСЕГО СПИСКА ОШИБОК ДЛЯ ПОДСВЕТКИ
            Console.WriteLine($"Принятое слово Yn: {FormatVector(received, errorPositionsIntroduced)}");
        }
        else
        {
            Console.WriteLine("Ошибок не внесено.");
            Console.WriteLine($"Принятое слово Yn: {FormatVector(received)}");
        }
        Console.WriteLine();

        int[] syndrome = ComputeSyndrome(H, received);
        Console.WriteLine(STEP_PREFIX + $"Синдром S = {FormatVector(syndrome)}");
        int errorPosDetected = FindErrorPosition(syndrome, H);
        Console.WriteLine();

        if (errorPosDetected == -1)
        {
            Console.WriteLine("Результат декодирования: Ошибок не обнаружено.");
            PrintStatus("Проверка статуса......: ", success: errorCount == 0);
            if (errorCount != 0) Console.WriteLine($"Комментарий: {errorCount} ошибок(ки) не обнаружено(ы)!");
        }
        else if (errorPosDetected == -2)
        {
            Console.WriteLine("Результат декодирования: Обнаружено >= 2 ошибок.");
            PrintStatus("Проверка статуса......: ", success: errorCount >= 2);
            if (errorCount < 2) Console.WriteLine($"Комментарий: Ожидалось {errorCount} ошибок, но синдром не найден.");
        }
        else
        {
            Console.WriteLine($"Результат декодирования: Обнаружена предполагаемая ошибка в позиции: {errorPosDetected}");
            int[] corrected = CorrectSingleError(received, errorPosDetected);
            int[] errorVector = new int[n];
            if (errorPosDetected >= 0) errorVector[errorPosDetected] = 1; // Защита на всякий случай
            Console.WriteLine($"Унарный вектор ошибки En: {FormatVector(errorVector, new List<int> { errorPosDetected })}"); // Подсвечиваем найденную ошибку
            Console.WriteLine($"Исправленное слово Xn': {FormatVector(corrected)}");

            bool correctionSuccess = (errorCount == 1 && errorPosDetected == errorPositionsIntroduced[0] && corrected.SequenceEqual(codeword));
            PrintStatus("Проверка статуса......: ", success: correctionSuccess);
            if (!correctionSuccess)
            {
                if (errorCount != 1) Console.WriteLine($"Комментарий: Неверное исправление (было {errorCount} ошибок).");
                else if (errorPosDetected != errorPositionsIntroduced[0]) Console.WriteLine($"Комментарий: Найдена неверная позиция ({errorPosDetected} вместо {errorPositionsIntroduced[0]}).");
                else Console.WriteLine("Комментарий: Позиция верна, но слово не восстановлено.");
            }
        }
        Console.WriteLine(new string('-', 50));
    }

    static void Main()
    {
        Console.OutputEncoding = System.Text.Encoding.UTF8;

        int r = 5;
        int n = 31;
        int k = n - r;
        int[] g = { 1, 0, 0, 1, 0, 1 };

        Console.WriteLine("\n--- 1. Параметры Кода ---");
        Console.WriteLine($"Длина кодового слова n: {n}");
        Console.WriteLine($"Длина инф. слова k  : {k}");
        Console.WriteLine($"Число провер. бит r  : {r}");
        Console.WriteLine($"Порождающий полином g(x): {string.Join("", g)}");
        if (g.Length != r + 1) { Console.WriteLine($"{ERROR_STATUS} Длина полинома g ({g.Length}) не равна r+1 ({r + 1})!"); return; }


        Console.WriteLine("\n--- 2. Генерация Матриц ---");
        int[,] G_noncanonical = GenerateGeneratorMatrixNonCanonical(k, n, g);
        PrintMatrixStyled("Неканоническая порождающая матрица", G_noncanonical);

        int[,] G_to_canonize = (int[,])G_noncanonical.Clone();
        ToCanonicalForm(G_to_canonize, k, n);
        int[,] G_canonical = G_to_canonize;
        PrintMatrixStyled("Каноническая порождающая матрица G = [Ik | P]", G_canonical, k);

        Console.WriteLine("Генерация проверочной матрицы H = [P^T | Ir]...");
        int[,] H = GenerateHMatrix(G_canonical, k, n);
        PrintMatrixStyled("Проверочная матрица H = [P^T | Ir]", H, k);

        Console.WriteLine("\n--- 3. Генерация Информационного Слова и Кодирование ---");
        Random random = new Random();
        int[] infoWord = new int[k];
        Console.Write("Генерация случайного Xk... ");
        for (int i = 0; i < k; i++) infoWord[i] = random.Next(0, 2);
        Console.WriteLine(OK_STATUS);
        Console.WriteLine($"Сгенерированное слово Xk: {FormatVector(infoWord)}");

        Console.Write("Кодирование Xn = Xk * G... ");
        int[] codeword = MultiplyVectorByGeneratorMatrix(infoWord, G_canonical);
        Console.WriteLine(OK_STATUS);
        Console.WriteLine($"Полученное слово Xn    : {FormatVector(codeword)}");


        Console.WriteLine("\n--- 4. Моделирование Ошибок и Декодирование ---");
        for (int errCount = 0; errCount <= 2; errCount++)
        {
            AnalyzeWithErrorCount(codeword, H, errCount, n, random);
        }

        Console.WriteLine("Нажмите Enter для выхода...");
        Console.ReadLine();
    }
}