using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

// --- Вспомогательный класс для вывода ---
public static class PrintUtils
{
    public static string FormatBinaryVector(IEnumerable<int> vector)
    {
        if (vector == null) return "null";
        // Добавим пробелы для читаемости длинных векторов
        // return string.Join("", vector);
        var sb = new StringBuilder();
        int count = 0;
        foreach (var bit in vector)
        {
            sb.Append(bit);
            count++;
            if (count % 8 == 0 && count < vector.Count()) sb.Append(" "); // Пробел каждые 8 бит
        }
        return sb.ToString();
    }

    public static void PrintVector(IEnumerable<int> vector, string label)
    {
        if (vector == null)
        {
            Console.WriteLine($"{label}: null");
            return;
        }
        Console.WriteLine($"{label}: [{FormatBinaryVector(vector)}]");
    }

    public static void PrintMatrix(int[,] matrix, string label)
    {
        if (matrix == null) { Console.WriteLine($"{label}: null"); return; }
        Console.WriteLine($"{label}:");
        int rows = matrix.GetLength(0);
        int cols = matrix.GetLength(1);
        for (int i = 0; i < rows; i++)
        {
            Console.Write("  [");
            for (int j = 0; j < cols; j++)
            {
                Console.Write(matrix[i, j]);
                if (j < cols - 1) Console.Write(", ");
            }
            Console.WriteLine("]");
        }
    }

    public static void PrintMatrix(int[,,] matrix, string label)
    {
        if (matrix == null) { Console.WriteLine($"{label}: null"); return; }
        Console.WriteLine($"{label}:");
        int dim1 = matrix.GetLength(0); // k1
        int dim2 = matrix.GetLength(1); // k2
        int dim3 = matrix.GetLength(2); // z

        for (int k = 0; k < dim3; k++) // Layer (z)
        {
            Console.WriteLine($"  Layer {k}:");
            for (int i = 0; i < dim1; i++)  // Row (k1)
            {
                Console.Write("    [");
                for (int j = 0; j < dim2; j++) // Column (k2)
                {
                    Console.Write(matrix[i, j, k]);
                    if (j < dim2 - 1) Console.Write(", ");
                }
                Console.WriteLine("]");
            }
        }
    }
}

// --- Класс итеративного кода (с улучшенным Decode и verbose в Encode) ---
public class IterativeCode
{
    public int K1 { get; } // Dimension 1
    public int K2 { get; } // Dimension 2
    public int? Z { get; } // Dimension 3 (nullable)
    public int NumParityGroups { get; } // Количество паритетных групп
    public int K { get; }
    public int R { get; private set; }
    public int N { get; private set; } // k + r

    private int[] _informationWord; // Xk
    private int[] _codewordXn;      // Xn
    private int[] _receivedWordYn;    // Yn (копия принятого слова для справки)
    private int[] _correctedWordYnPrime; // Yn после исправления

    private int[,] _matrix2D;
    private int[,,] _matrix3D;

    // Внутренние поля для хранения ПЕРЕСЧИТАННЫХ битов четности после кодирования/декодирования
    private int[] _parityGroup1;
    private int[] _parityGroup2;
    private int[] _parityGroup3;
    private int[] _parityGroup4;

    private readonly Random _random = new Random();
    private const int MAX_DECODING_ITERATIONS = 10;

    public IterativeCode(int k1, int k2, int? z, int numParityGroups)
    {
        K1 = k1;
        K2 = k2;
        Z = z;
        NumParityGroups = numParityGroups;

        if (Z.HasValue) { K = K1 * K2 * Z.Value; } else { K = K1 * K2; }
        CalculateRedundancy();
        N = K + R;
    }

    private void CalculateRedundancy()
    {
        R = 0;
        if (!Z.HasValue) // 2D case
        {
            if (NumParityGroups >= 1) R += K1; // Row parity bits
            if (NumParityGroups >= 2) R += K2; // Column parity bits
            if (NumParityGroups == 1) R = K1;
        }
        else // 3D Case
        {
            int actualR = 0;
            if (NumParityGroups >= 1) actualR += K2 * Z.Value; // Parity along k1
            if (NumParityGroups >= 2) actualR += K1 * Z.Value; // Parity along k2
            if (NumParityGroups >= 3) actualR += K1 * K2;     // Parity along z
            if (NumParityGroups >= 4) actualR += 1;         // Overall parity
            R = actualR;
        }
    }

    private void FillMatrix(int[] data)
    {
        int index = 0;
        if (_matrix2D != null)
        {
            for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) _matrix2D[i, j] = data[index++];
        }
        else if (_matrix3D != null)
        {
            for (int k = 0; k < Z.Value; k++) for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) _matrix3D[i, j, k] = data[index++];
        }
        else
        {
            throw new InvalidOperationException("Matrix not initialized before FillMatrix.");
        }
    }

    private int CalculateParity(IEnumerable<int> bits) => bits.Aggregate(0, (acc, bit) => acc ^ bit);

    // Рассчитывает _parityGroupX на основе текущей матрицы (_matrix2D или _matrix3D)
    private void CalculateParityBits()
    {
        _parityGroup1 = null; _parityGroup2 = null; _parityGroup3 = null; _parityGroup4 = null;

        if (_matrix2D != null) // 2D Case
        {
            if (NumParityGroups >= 1)
            {
                _parityGroup1 = new int[K1];
                for (int i = 0; i < K1; i++) _parityGroup1[i] = CalculateParity(Enumerable.Range(0, K2).Select(j => _matrix2D[i, j]));
            }
            if (NumParityGroups >= 2)
            {
                _parityGroup2 = new int[K2];
                for (int j = 0; j < K2; j++) _parityGroup2[j] = CalculateParity(Enumerable.Range(0, K1).Select(i => _matrix2D[i, j]));
            }
        }
        else if (_matrix3D != null)// 3D Case
        {
            if (NumParityGroups >= 1)
            {
                _parityGroup1 = new int[K2 * Z.Value];
                int p1Idx = 0;
                for (int k = 0; k < Z.Value; k++) for (int j = 0; j < K2; j++) _parityGroup1[p1Idx++] = CalculateParity(Enumerable.Range(0, K1).Select(i => _matrix3D[i, j, k]));
            }
            if (NumParityGroups >= 2)
            {
                _parityGroup2 = new int[K1 * Z.Value];
                int p2Idx = 0;
                for (int k = 0; k < Z.Value; k++) for (int i = 0; i < K1; i++) _parityGroup2[p2Idx++] = CalculateParity(Enumerable.Range(0, K2).Select(j => _matrix3D[i, j, k]));
            }
            if (NumParityGroups >= 3)
            {
                _parityGroup3 = new int[K1 * K2];
                int p3Idx = 0;
                for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) _parityGroup3[p3Idx++] = CalculateParity(Enumerable.Range(0, Z.Value).Select(k => _matrix3D[i, j, k]));
            }
            if (NumParityGroups >= 4)
            {
                _parityGroup4 = new int[1];
                // Четность по информационным битам _informationWord
                _parityGroup4[0] = CalculateParity(_informationWord);
            }
        }
        else
        {
            throw new InvalidOperationException("Matrix not initialized before CalculateParityBits.");
        }
    }

    // Собирает _codewordXn из _informationWord и текущих _parityGroupX
    private void FormCodewordXn()
    {
        var codewordList = new List<int>(_informationWord);
        if (_parityGroup1 != null) codewordList.AddRange(_parityGroup1);
        if (_parityGroup2 != null) codewordList.AddRange(_parityGroup2);
        if (_parityGroup3 != null) codewordList.AddRange(_parityGroup3);
        if (_parityGroup4 != null) codewordList.AddRange(_parityGroup4);
        _codewordXn = codewordList.ToArray();

        // Проверка соответствия длины N
        if (_codewordXn.Length != N)
        {
            N = _codewordXn.Length; // Корректируем N по факту
            R = N - K; // Корректируем R по факту
        }
    }

    // Вносит пакетную ошибку
    public int[] IntroduceBurstError(int[] originalCodeword, int burstLength, int startIndex)
    {
        if (originalCodeword == null) throw new InvalidOperationException("Original codeword is null.");
        if (burstLength <= 0) return (int[])originalCodeword.Clone(); // No error
        if (startIndex < 0 || startIndex >= originalCodeword.Length)
        {
            throw new ArgumentOutOfRangeException(nameof(startIndex), $"Start index {startIndex} is out of bounds for codeword length {originalCodeword.Length}.");
        }
        // Проверка конца пакета
        int effectiveBurstLength = Math.Min(burstLength, originalCodeword.Length - startIndex);

        int[] receivedWord = (int[])originalCodeword.Clone(); // Работаем с копией

        for (int i = 0; i < effectiveBurstLength; i++)
        {
            int errorPosition = startIndex + i;
            receivedWord[errorPosition] = 1 - receivedWord[errorPosition]; // Flip the bit
        }
        // Сохраняем копию принятого слова с ошибкой для возможной отладки
        _receivedWordYn = (int[])receivedWord.Clone();
        return receivedWord; // Возвращаем слово с ошибкой
    }

    // --- Encode (С ПАРАМЕТРОМ verbose) ---
    public int[] Encode(int[] informationWord, bool verbose = false) // <-- Добавлен verbose
    {
        if (informationWord == null || informationWord.Length != K)
        {
            throw new ArgumentException($"Information word must have length {K}. Got {informationWord?.Length ?? 0}");
        }
        _informationWord = (int[])informationWord.Clone();

        if (verbose)
        {
            Console.WriteLine("\n--- Кодирование Блока ---");
            PrintUtils.PrintVector(_informationWord, "Информационное слово (Xk)");
        }

        // Инициализируем матрицы здесь
        if (!Z.HasValue) _matrix2D = new int[K1, K2]; else _matrix3D = new int[K1, K2, Z.Value];

        // 1. Заполнение матрицы
        FillMatrix(_informationWord);
        if (verbose)
        {
            if (_matrix2D != null) PrintUtils.PrintMatrix(_matrix2D, "Матрица данных (Заполнена Xk)");
            else if (_matrix3D != null) PrintUtils.PrintMatrix(_matrix3D, "Матрица данных (3D) (Заполнена Xk)");
        }

        // 2. Вычисление проверочных битов
        CalculateParityBits(); // Рассчитает _parityGroupX на основе матрицы
        if (verbose)
        {
            Console.WriteLine("Рассчитанные проверочные биты (Xr):");
            PrintUtils.PrintVector(GetParityGroup(1), "  P1 (Строки / K1-dir)"); // Используем Getters для вывода
            PrintUtils.PrintVector(GetParityGroup(2), "  P2 (Столбцы / K2-dir)");
            if (Z.HasValue)
            {
                PrintUtils.PrintVector(GetParityGroup(3), "  P3 (Глубина / Z-dir)");
                PrintUtils.PrintVector(GetParityGroup(4), "  P4 (Общий)");
            }
        }

        // 3. Формирование кодового слова
        FormCodewordXn(); // Соберет _codewordXn из _informationWord и _parityGroupX
        if (verbose)
        {
            PrintUtils.PrintVector(_codewordXn, "Итоговое кодовое слово (Xn = Xk + Xr)");
            Console.WriteLine("--- Конец Кодирования Блока ---");
        }
        return (int[])_codewordXn.Clone();
    }


    // --- Decode (Улучшенный, с verboseOutput) ---
    public int[] Decode(int[] receivedWord, bool verboseOutput = false)
    {
        if (receivedWord == null || receivedWord.Length != N)
        {
            int expectedN = K + R; // N рассчитанный при инициализации
            Console.WriteLine($"Decode Error: Received word length mismatch. Expected N={N} (K={K}, R={R}), Received={receivedWord?.Length ?? -1}. Initial expected N was {expectedN}.");
            throw new ArgumentException($"Received word must have length {N}. Got {receivedWord?.Length ?? 0}");
        }

        // Сохраняем копию для отладки
        _receivedWordYn = (int[])receivedWord.Clone();

        // Рабочие копии данных и проверочных битов
        int[] currentData = receivedWord.Take(K).ToArray();

        // Определяем длины ОЖИДАЕМЫХ групп четности на основе параметров КОДЕРА
        int p1Len = 0, p2Len = 0, p3Len = 0, p4Len = 0;
        if (!Z.HasValue) // 2D
        {
            if (NumParityGroups >= 1) p1Len = K1;
            if (NumParityGroups >= 2) p2Len = K2;
        }
        else
        { // 3D
            if (NumParityGroups >= 1) p1Len = K2 * Z.Value;
            if (NumParityGroups >= 2) p2Len = K1 * Z.Value;
            if (NumParityGroups >= 3) p3Len = K1 * K2;
            if (NumParityGroups >= 4) p4Len = 1;
        }
        // Проверка, что сумма длин K + p1..p4 равна N
        if (K + p1Len + p2Len + p3Len + p4Len != N)
        {
            Console.WriteLine($"Decode Warning: Sum of K ({K}) and expected parity lengths (P1:{p1Len}, P2:{p2Len}, P3:{p3Len}, P4:{p4Len}) = {K + p1Len + p2Len + p3Len + p4Len}, which does not match N ({N}). Check CalculateRedundancy.");
            // Это может привести к IndexOutOfRangeException ниже
        }

        // Извлекаем ПРИНЯТЫЕ проверочные биты и СОЗДАЕМ ИХ РАБОЧИЕ КОПИИ
        int[] currentReceivedP1 = receivedWord.Skip(K).Take(p1Len).ToArray();
        int[] currentReceivedP2 = receivedWord.Skip(K + p1Len).Take(p2Len).ToArray();
        int[] currentReceivedP3 = receivedWord.Skip(K + p1Len + p2Len).Take(p3Len).ToArray();
        int[] currentReceivedP4 = receivedWord.Skip(K + p1Len + p2Len + p3Len).Take(p4Len).ToArray();

        // Рабочая матрица
        int[,] currentMatrix2D = null;
        int[,,] currentMatrix3D = null; // 3D пока не поддерживается улучшенным декодером

        if (!Z.HasValue)
        {
            currentMatrix2D = new int[K1, K2];
            FillMatrix2DFromData(currentData, currentMatrix2D);
        }
        else
        {
            // Инициализация для 3D (но декодирование будет базовым)
            currentMatrix3D = new int[K1, K2, Z.Value];
            FillMatrix3DFromData(currentData, currentMatrix3D);
            if (verboseOutput) Console.WriteLine("Предупреждение: Улучшенное итеративное декодирование пока реализовано только для 2D.");
        }

        if (verboseOutput)
        {
            Console.WriteLine("\n--- Начало Декодирования Блока ---");
            PrintUtils.PrintVector(receivedWord, "Принятое слово (Yn)");
            if (currentMatrix2D != null) PrintUtils.PrintMatrix(currentMatrix2D, "Начальная матрица Yk");
            else if (currentMatrix3D != null) PrintUtils.PrintMatrix(currentMatrix3D, "Начальная матрица Yk (3D)");
            PrintUtils.PrintVector(currentReceivedP1, "Принятые P1 (Yr1)");
            PrintUtils.PrintVector(currentReceivedP2, "Принятые P2 (Yr2)");
            // Вывод P3, P4 для 3D
            if (Z.HasValue)
            {
                PrintUtils.PrintVector(currentReceivedP3, "Принятые P3 (Yr3)");
                PrintUtils.PrintVector(currentReceivedP4, "Принятые P4 (Yr4)");
            }
        }

        // --- Итеративный Цикл Декодирования ---
        bool decodingStuck = false;
        for (int iter = 0; iter < MAX_DECODING_ITERATIONS; iter++)
        {
            if (verboseOutput) Console.WriteLine($"\n--- Итерация {iter + 1} ---");

            bool correctionMadeThisIteration = false;

            // --- Вычисление Синдромов на основе ТЕКУЩЕЙ матрицы и ТЕКУЩИХ принятых проверочных битов ---
            int[] syndrome1 = null, syndrome2 = null, syndrome3 = null, syndrome4 = null;

            // Используем именно рабочие копии currentReceivedPX !
            if (!Z.HasValue && currentMatrix2D != null) // 2D Case
            {
                if (NumParityGroups >= 1) syndrome1 = CalculateSyndrome2D(currentMatrix2D, currentReceivedP1, 1);
                if (NumParityGroups >= 2) syndrome2 = CalculateSyndrome2D(currentMatrix2D, currentReceivedP2, 2);

                if (verboseOutput)
                {
                    PrintUtils.PrintVector(syndrome1, "Синдром S1 (строки)");
                    PrintUtils.PrintVector(syndrome2, "Синдром S2 (столбцы)");
                }
            }
            else if (Z.HasValue && currentMatrix3D != null) // 3D Case (использует базовые синдромы)
            {
                if (NumParityGroups >= 1) syndrome1 = CalculateSyndrome3D(currentMatrix3D, currentReceivedP1, 1);
                if (NumParityGroups >= 2) syndrome2 = CalculateSyndrome3D(currentMatrix3D, currentReceivedP2, 2);
                if (NumParityGroups >= 3) syndrome3 = CalculateSyndrome3D(currentMatrix3D, currentReceivedP3, 3);
                if (NumParityGroups >= 4) syndrome4 = CalculateSyndrome3D(currentMatrix3D, currentReceivedP4, 4); // Использует currentData из FlattenMatrix
                if (verboseOutput)
                {
                    PrintUtils.PrintVector(syndrome1, "Синдром S1 (k1-dir)");
                    PrintUtils.PrintVector(syndrome2, "Синдром S2 (k2-dir)");
                    PrintUtils.PrintVector(syndrome3, "Синдром S3 (z-dir)");
                    PrintUtils.PrintVector(syndrome4, "Синдром S4 (overall)");
                }
            }
            else
            {
                if (verboseOutput) Console.WriteLine("Матрица не инициализирована.");
                decodingStuck = true; // Прерываем декодирование
                break;
            }

            // --- Проверка Сходимости ---
            bool allZero = (syndrome1?.All(s => s == 0) ?? true) &&
                           (syndrome2?.All(s => s == 0) ?? true) &&
                           (syndrome3?.All(s => s == 0) ?? true) &&
                           (syndrome4?.All(s => s == 0) ?? true);

            if (allZero)
            {
                if (verboseOutput) Console.WriteLine("Все синдромы нулевые. Декодирование успешно.");
                break;
            }

            // --- Идентификация и Исправление Ошибок (Фокус на 2D, NumParityGroups >= 2) ---
            if (!Z.HasValue && currentMatrix2D != null && NumParityGroups >= 2 && syndrome1 != null && syndrome2 != null)
            {
                // Создаем копии синдромов, чтобы модифицировать их по ходу исправления внутри итерации
                int[] currentSyndrome1 = (int[])syndrome1.Clone();
                int[] currentSyndrome2 = (int[])syndrome2.Clone();

                // Этап 1: Исправление ошибок в данных (по пересечению синдромов)
                for (int i = 0; i < K1; i++)
                {
                    for (int j = 0; j < K2; j++)
                    {
                        if (currentSyndrome1[i] == 1 && currentSyndrome2[j] == 1)
                        {
                            if (verboseOutput) Console.WriteLine($"  Исправление данных в ({i},{j})");
                            currentMatrix2D[i, j] = 1 - currentMatrix2D[i, j]; // Исправляем бит в матрице
                            correctionMadeThisIteration = true;

                            // "Поглощаем" синдромы, которые объяснились этой ошибкой
                            currentSyndrome1[i] = 0;
                            currentSyndrome2[j] = 0;
                        }
                    }
                }

                // Этап 2: Исправление ошибок в проверочных битах строк (P1)
                for (int i = 0; i < K1; i++)
                {
                    // Если синдром строки все еще 1 после этапа 1
                    if (currentSyndrome1[i] == 1)
                    {
                        if (verboseOutput) Console.WriteLine($"  Исправление проверочного бита P1[{i}]");
                        currentReceivedP1[i] = 1 - currentReceivedP1[i]; // Исправляем РАБОЧУЮ КОПИЮ принятого P1
                        correctionMadeThisIteration = true;
                        // Не обнуляем currentSyndrome1[i] здесь, т.к. он будет пересчитан в след. итерации
                    }
                }

                // Этап 3: Исправление ошибок в проверочных битах столбцов (P2)
                for (int j = 0; j < K2; j++)
                {
                    if (currentSyndrome2[j] == 1) // Если синдром столбца все еще 1 после этапа 1 и 2
                    {
                        if (verboseOutput) Console.WriteLine($"  Исправление проверочного бита P2[{j}]");
                        currentReceivedP2[j] = 1 - currentReceivedP2[j]; // Исправляем РАБОЧУЮ КОПИЮ принятого P2
                        correctionMadeThisIteration = true;
                        // Не обнуляем currentSyndrome2[j] здесь
                    }
                }
            }
            else if (Z.HasValue && currentMatrix3D != null)
            {
                // --- Оставляем старую (неполную) логику исправления для 3D ---
                for (int i = 0; i < K1; i++)
                {
                    for (int j = 0; j < K2; j++)
                    {
                        for (int k = 0; k < Z.Value; k++)
                        {
                            int failingChecks = 0;
                            int s1_idx = k * K2 + j; int s2_idx = k * K1 + i; int s3_idx = i * K2 + j;
                            if (syndrome1 != null && s1_idx < syndrome1.Length && syndrome1[s1_idx] == 1) failingChecks++;
                            if (syndrome2 != null && s2_idx < syndrome2.Length && syndrome2[s2_idx] == 1) failingChecks++;
                            if (syndrome3 != null && s3_idx < syndrome3.Length && syndrome3[s3_idx] == 1) failingChecks++;
                            int requiredChecks = (NumParityGroups >= 3) ? 2 : ((NumParityGroups == 2) ? 2 : 1); // Minimum checks needed
                            if (requiredChecks == 1 && NumParityGroups < 2) requiredChecks = 2; // Need at least 2 for 3D correction usually

                            if (failingChecks >= requiredChecks)
                            {
                                if (verboseOutput) Console.WriteLine($"  3D Коррекция данных в ({i},{j},{k})");
                                currentMatrix3D[i, j, k] = 1 - currentMatrix3D[i, j, k];
                                correctionMadeThisIteration = true;
                                // TODO: Implement syndrome absorption for 3D if making corrections
                            }
                        }
                    }
                }
            }


            // --- Проверка на застревание ---
            if (!correctionMadeThisIteration && !allZero)
            {
                if (verboseOutput) Console.WriteLine("Не удалось внести исправления на этой итерации, но синдромы ненулевые. Остановка (неисправляемая ошибка?).");
                decodingStuck = true; // Помечаем, что застряли
                break;
            }

            // --- Выход при макс. итерациях ---
            if (iter == MAX_DECODING_ITERATIONS - 1 && !allZero)
            {
                if (verboseOutput) Console.WriteLine($"Достигнуто макс. число итераций ({MAX_DECODING_ITERATIONS}), ошибки могут остаться.");
                decodingStuck = true; // Считаем, что застряли
            }

        } // --- Конец Итеративного Цикла ---

        // --- Реконструкция Финального Слова Yn' ---
        int[] finalCorrectedData;
        int[] finalCalculatedP1 = null;
        int[] finalCalculatedP2 = null;
        int[] finalCalculatedP3 = null;
        int[] finalCalculatedP4 = null;

        if (!Z.HasValue && currentMatrix2D != null)
        {
            finalCorrectedData = FlattenMatrix(currentMatrix2D);
            // Пересчет P1
            if (NumParityGroups >= 1 && p1Len > 0)
            {
                finalCalculatedP1 = new int[p1Len]; // Используем p1Len для корректного размера
                for (int i = 0; i < K1; i++) finalCalculatedP1[i] = CalculateParity(Enumerable.Range(0, K2).Select(j => currentMatrix2D[i, j]));
            }
            // Пересчет P2
            if (NumParityGroups >= 2 && p2Len > 0)
            {
                finalCalculatedP2 = new int[p2Len];
                for (int j = 0; j < K2; j++) finalCalculatedP2[j] = CalculateParity(Enumerable.Range(0, K1).Select(i => currentMatrix2D[i, j]));
            }
        }
        else if (Z.HasValue && currentMatrix3D != null)
        {
            finalCorrectedData = FlattenMatrix(currentMatrix3D);
            // Пересчет для 3D (полный)
            if (NumParityGroups >= 1 && p1Len > 0)
            {
                finalCalculatedP1 = new int[p1Len];
                int p1Idx = 0; for (int k = 0; k < Z.Value; k++) for (int j = 0; j < K2; j++) finalCalculatedP1[p1Idx++] = CalculateParity(Enumerable.Range(0, K1).Select(i => currentMatrix3D[i, j, k]));
            }
            if (NumParityGroups >= 2 && p2Len > 0)
            {
                finalCalculatedP2 = new int[p2Len];
                int p2Idx = 0; for (int k = 0; k < Z.Value; k++) for (int i = 0; i < K1; i++) finalCalculatedP2[p2Idx++] = CalculateParity(Enumerable.Range(0, K2).Select(j => currentMatrix3D[i, j, k]));
            }
            if (NumParityGroups >= 3 && p3Len > 0)
            {
                finalCalculatedP3 = new int[p3Len];
                int p3Idx = 0; for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) finalCalculatedP3[p3Idx++] = CalculateParity(Enumerable.Range(0, Z.Value).Select(k => currentMatrix3D[i, j, k]));
            }
            if (NumParityGroups >= 4 && p4Len > 0)
            {
                finalCalculatedP4 = new int[p4Len];
                // Четность по ИСПРАВЛЕННЫМ данным
                finalCalculatedP4[0] = CalculateParity(finalCorrectedData);
            }
        }
        else
        {
            // Не удалось инициализировать матрицу - возвращаем исходное слово
            if (verboseOutput) Console.WriteLine("Ошибка: Декодирование не выполнено из-за отсутствия матрицы.");
            _correctedWordYnPrime = (int[])receivedWord.Clone();
            return _correctedWordYnPrime;
        }

        // Сборка финального Yn' = Исправленные Данные + Пересчитанные Проверки
        var correctedList = new List<int>(finalCorrectedData);
        if (finalCalculatedP1 != null) correctedList.AddRange(finalCalculatedP1);
        if (finalCalculatedP2 != null) correctedList.AddRange(finalCalculatedP2);
        if (finalCalculatedP3 != null) correctedList.AddRange(finalCalculatedP3);
        if (finalCalculatedP4 != null) correctedList.AddRange(finalCalculatedP4);
        _correctedWordYnPrime = correctedList.ToArray();

        // Финальная проверка длины
        if (_correctedWordYnPrime.Length != N)
        {
            Console.WriteLine($"КРИТИЧЕСКАЯ ОШИБКА: Длина финального исправленного слова ({_correctedWordYnPrime.Length}) не совпадает с N ({N}).");
            // Вернем исходное слово в случае ошибки
            _correctedWordYnPrime = (int[])receivedWord.Clone();
        }
        else if (decodingStuck && verboseOutput)
        {
            Console.WriteLine("Предупреждение: Декодирование остановлено из-за зацикливания или лимита итераций. Результат может быть неверным.");
        }

        if (verboseOutput)
        {
            Console.WriteLine("\n--- Конец Декодирования Блока ---");
            if (currentMatrix2D != null) PrintUtils.PrintMatrix(currentMatrix2D, "Финальная матрица данных (после испр.)");
            else if (currentMatrix3D != null) PrintUtils.PrintMatrix(currentMatrix3D, "Финальная матрица данных (3D, после испр.)");
            PrintUtils.PrintVector(finalCalculatedP1, "Пересчитанные P1");
            PrintUtils.PrintVector(finalCalculatedP2, "Пересчитанные P2");
            if (Z.HasValue) { PrintUtils.PrintVector(finalCalculatedP3, "Пересчитанные P3"); PrintUtils.PrintVector(finalCalculatedP4, "Пересчитанные P4"); }
            PrintUtils.PrintVector(_correctedWordYnPrime, "Финальное исправленное слово (Yn')");
        }

        return (int[])_correctedWordYnPrime.Clone();
    }


    // --- Вспомогательные методы для декодирования ---
    private void FillMatrix2DFromData(int[] data, int[,] matrix)
    {
        int index = 0;
        if (data.Length != K) throw new ArgumentException($"Data length {data.Length} != K {K}");
        for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) matrix[i, j] = data[index++];
    }

    private void FillMatrix3DFromData(int[] data, int[,,] matrix)
    {
        int index = 0;
        if (data.Length != K) throw new ArgumentException($"Data length {data.Length} != K {K}");
        for (int k = 0; k < Z.Value; k++) for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) matrix[i, j, k] = data[index++];
    }

    private int[] FlattenMatrix(int[,] matrix)
    {
        int[] data = new int[K];
        int index = 0;
        for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) data[index++] = matrix[i, j];
        return data;
    }

    private int[] FlattenMatrix(int[,,] matrix)
    {
        int[] data = new int[K];
        int index = 0;
        for (int k = 0; k < Z.Value; k++) for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) data[index++] = matrix[i, j, k];
        return data;
    }

    // --- CalculateSyndrome - Robust к null/длине ---
    private int[] CalculateSyndrome2D(int[,] matrix, int[] receivedParity, int groupIndex)
    {
        int expectedLen = (groupIndex == 1) ? K1 : K2;
        if (matrix == null || receivedParity == null || receivedParity.Length != expectedLen)
        {
            // Console.WriteLine($"Warning: CalculateSyndrome2D G{groupIndex} input error. Matrix null: {matrix == null}, ReceivedParity null: {receivedParity == null}, Len mismatch: {receivedParity?.Length ?? -1} != {expectedLen}");
            return Enumerable.Repeat(0, expectedLen).ToArray(); // Возвращаем нулевой синдром при ошибке
        }

        int[] calculatedParity = new int[expectedLen];
        int[] syndrome = new int[expectedLen];

        try
        {
            if (groupIndex == 1) // Row Parity Syndrome
            {
                for (int i = 0; i < K1; i++) calculatedParity[i] = CalculateParity(Enumerable.Range(0, K2).Select(j => matrix[i, j]));
            }
            else // Column Parity Syndrome (groupIndex == 2)
            {
                for (int j = 0; j < K2; j++) calculatedParity[j] = CalculateParity(Enumerable.Range(0, K1).Select(i => matrix[i, j]));
            }
            // XOR для получения синдрома
            for (int i = 0; i < expectedLen; ++i) syndrome[i] = calculatedParity[i] ^ receivedParity[i];
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error calculating 2D syndrome G{groupIndex}: {ex.Message}");
            Array.Fill(syndrome, 0); // Нулевой синдром при исключении
        }
        return syndrome;
    }

    private int[] CalculateSyndrome3D(int[,,] matrix, int[] receivedParity, int groupIndex)
    {
        if (matrix == null)
        {
            return null; 
        }

        int expectedLen = 0;
        switch (groupIndex)
        {
            case 1: expectedLen = K2 * Z.Value; break;
            case 2: expectedLen = K1 * Z.Value; break;
            case 3: expectedLen = K1 * K2; break;
            case 4: expectedLen = 1; break;
            default: return new int[0]; // Invalid group
        }

        if (receivedParity == null || receivedParity.Length != expectedLen)
        {
            // Console.WriteLine($"Warning: CalculateSyndrome3D G{groupIndex} input error. ReceivedParity null: {receivedParity == null}, Len mismatch: {receivedParity?.Length ?? -1} != {expectedLen}");
            return Enumerable.Repeat(0, expectedLen).ToArray(); // Нулевой синдром
        }

        int[] calculatedParity = new int[expectedLen];
        int[] syndrome = new int[expectedLen];

        try
        {
            switch (groupIndex)
            {
                case 1: // k1 direction
                    int p1Idx = 0; for (int k = 0; k < Z.Value; k++) for (int j = 0; j < K2; j++) calculatedParity[p1Idx++] = CalculateParity(Enumerable.Range(0, K1).Select(i => matrix[i, j, k]));
                    break;
                case 2: // k2 direction
                    int p2Idx = 0; for (int k = 0; k < Z.Value; k++) for (int i = 0; i < K1; i++) calculatedParity[p2Idx++] = CalculateParity(Enumerable.Range(0, K2).Select(j => matrix[i, j, k]));
                    break;
                case 3: // z direction
                    int p3Idx = 0; for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) calculatedParity[p3Idx++] = CalculateParity(Enumerable.Range(0, Z.Value).Select(k => matrix[i, j, k]));
                    break;
                case 4: // overall info parity
                    // Рассчитываем по ТЕКУЩИМ данным в матрице
                    int[] currentData = FlattenMatrix(matrix);
                    calculatedParity[0] = CalculateParity(currentData);
                    break;
            }
            // XOR для синдрома
            for (int i = 0; i < expectedLen; ++i) syndrome[i] = calculatedParity[i] ^ receivedParity[i];
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error calculating 3D syndrome G{groupIndex}: {ex.Message}");
            Array.Fill(syndrome, 0); // Нулевой синдром при исключении
        }
        return syndrome;
    }

    // --- AnalyzeCorrection ---
    public bool AnalyzeCorrection(int[] originalCodeword, int[] correctedWord)
    {
        if (originalCodeword == null || correctedWord == null) return false;
        if (originalCodeword.Length != correctedWord.Length)
        {
            return false;
        }
        return originalCodeword.SequenceEqual(correctedWord);
    }

    // --- Getters ---
    public int[] GetInformationWord() => (int[])_informationWord?.Clone();
    public int[] GetCodewordXn() => (int[])_codewordXn?.Clone();
    public int[] GetReceivedWordYn() => (int[])_receivedWordYn?.Clone(); // Получить копию слова с ошибкой
    public int[] GetCorrectedWordYnPrime() => (int[])_correctedWordYnPrime?.Clone();
    public int[,] GetMatrix2D() => (int[,])_matrix2D?.Clone();
    public int[,,] GetMatrix3D() => (int[,,])_matrix3D?.Clone();
    public int[] GetParityGroup(int groupNum)
    {
        switch (groupNum)
        {
            case 1: return (int[])_parityGroup1?.Clone();
            case 2: return (int[])_parityGroup2?.Clone();
            case 3: return (int[])_parityGroup3?.Clone();
            case 4: return (int[])_parityGroup4?.Clone();
            default: return null;
        }
    }

} 
// --- Класс перемежителя (с verbose) ---
public class Interleaver
{
    private readonly int _rows;
    private readonly int _columns;
    private readonly int _originalDataLength;

    public Interleaver(int columns, int dataLength)
    {
        if (columns <= 0) throw new ArgumentException("Number of columns must be positive.", nameof(columns));
        if (dataLength <= 0) throw new ArgumentException("Data length must be positive.", nameof(dataLength));

        _columns = columns;
        _originalDataLength = dataLength;
        _rows = (int)Math.Ceiling((double)dataLength / columns);
    }

    public int[] Interleave(int[] data, bool verbose = false)
    {
        if (data.Length != _originalDataLength)
            throw new ArgumentException($"Input data length {data.Length} does not match expected length {_originalDataLength}.", nameof(data));

        int paddedLength = _rows * _columns;
        int[,] matrix = new int[_rows, _columns];
        int dataIndex = 0;
        var interleavedData = new List<int>(paddedLength);

        if (verbose)
        {
            Console.WriteLine("\n--- Перемежение (Interleaving) ---");
            PrintUtils.PrintVector(data, "Входные данные (до перемежения)");
        }

        // Запись по строкам с паддингом
        for (int i = 0; i < _rows; i++)
        {
            for (int j = 0; j < _columns; j++)
            {
                if (dataIndex < _originalDataLength) matrix[i, j] = data[dataIndex++];
                else matrix[i, j] = 0; 
            }
        }

        if (verbose)
        {
            PrintUtils.PrintMatrix(matrix, $"Матрица перемежителя ({_rows}x{_columns}) (Заполнена по строкам)");
            Console.WriteLine("Чтение данных по столбцам...");
        }

        // Чтение по столбцам
        for (int j = 0; j < _columns; j++)
        {
            for (int i = 0; i < _rows; i++)
            {
                interleavedData.Add(matrix[i, j]);
            }
        }

        int[] result = interleavedData.ToArray();
        if (verbose)
        {
            PrintUtils.PrintVector(result, "Выходные данные (после перемежения)");
            Console.WriteLine("--- Конец Перемежения ---");
        }
        return result;
    }

    public int[] Deinterleave(int[] interleavedData, bool verbose = false)
    {
        int expectedInterleavedLength = _rows * _columns;
        if (interleavedData.Length != expectedInterleavedLength)
            throw new ArgumentException($"Input interleaved data length {interleavedData.Length} does not match expected length {expectedInterleavedLength}.", nameof(interleavedData));

        int[,] matrix = new int[_rows, _columns];
        int dataIndex = 0;
        var deinterleavedData = new List<int>(_originalDataLength);

        if (verbose)
        {
            Console.WriteLine("\n--- Деперемежение (Deinterleaving) ---");
            PrintUtils.PrintVector(interleavedData, "Входные данные (перемеженные, с ошибкой)");
        }

        // Запись по столбцам
        for (int j = 0; j < _columns; j++)
        {
            for (int i = 0; i < _rows; i++)
            {
                matrix[i, j] = interleavedData[dataIndex++];
            }
        }

        if (verbose)
        {
            PrintUtils.PrintMatrix(matrix, $"Матрица деперемежителя ({_rows}x{_columns}) (Заполнена по столбцам)");
            Console.WriteLine($"Чтение данных по строкам (до исходной длины {_originalDataLength})...");
        }

        // Чтение по строкам до originalDataLength
        dataIndex = 0;
        for (int i = 0; i < _rows; i++)
        {
            for (int j = 0; j < _columns; j++)
            {
                if (dataIndex < _originalDataLength)
                {
                    deinterleavedData.Add(matrix[i, j]);
                    dataIndex++;
                }
                else break;
            }
            if (dataIndex >= _originalDataLength) break;
        }

        int[] result = deinterleavedData.ToArray();
        if (verbose)
        {
            PrintUtils.PrintVector(result, "Выходные данные (деперемеженные, с 'размазанной' ошибкой)");
            Console.WriteLine("--- Конец Деперемежения ---");
        }
        return result;
    }
    public int GetInterleavedLength() => _rows * _columns;
}

class Program
{
    static readonly Random GlobalRandom = new Random();

    static int[] GenerateRandomBinaryWord(int length)
    {
        int[] word = new int[length];
        for (int i = 0; i < length; i++) word[i] = GlobalRandom.Next(2);
        return word;
    }

    static List<int[]> SegmentData(int[] data, int blockSize)
    {
        var blocks = new List<int[]>();
        int numberOfBlocks = (int)Math.Ceiling((double)data.Length / blockSize);
        for (int i = 0; i < numberOfBlocks; i++)
        {
            int currentBlockSize = Math.Min(blockSize, data.Length - i * blockSize);
            int[] block = new int[blockSize]; // Всегда создаем блок полной длины
            int copyLength = currentBlockSize;
            Array.Copy(data, i * blockSize, block, 0, copyLength);
            blocks.Add(block);
        }
        return blocks;
    }

    static int[] ConcatenateData(List<int[]> blocks)
    {
        if (blocks == null || blocks.Count == 0) return new int[0];
        int totalLength = blocks.Sum(b => b?.Length ?? 0); // Проверка на null
        int[] result = new int[totalLength];
        int currentIndex = 0;
        foreach (var block in blocks)
        {
            if (block != null)
            {
                Array.Copy(block, 0, result, currentIndex, block.Length);
                currentIndex += block.Length;
            }
        }
        return result;
    }

    // --- Main ---
    static void Main(string[] args)
    {
        const bool DETAILED_OUTPUT = true; // true - показывать детали, false - только итоги

        // --- Параметры Симуляции ---
        const int TOTAL_MESSAGE_BYTES = 15; const int K_INFO_BITS = 6;
        const int INTERLEAVER_COLUMNS = 6; const int NUM_TRIALS = 35; 
        int[] BURST_LENGTHS = { 3, 5, 6 };
        const int K1 = 2; const int K2 = 3; const int NUM_PARITY_GROUPS = 2;

        // --- Инициализация ---
        int totalMessageBits = TOTAL_MESSAGE_BYTES * 8; int numInfoBlocks = totalMessageBits / K_INFO_BITS;
        if (totalMessageBits % K_INFO_BITS != 0) { Console.WriteLine("Ошибка: Длина сообщения не делится на K_INFO_BITS!"); return; }
        var coder = new IterativeCode(K1, K2, null, NUM_PARITY_GROUPS);
        int n_encodedBlockLength = coder.N; int totalEncodedBits = numInfoBlocks * n_encodedBlockLength;
        var interleaver = new Interleaver(INTERLEAVER_COLUMNS, totalEncodedBits);
        int interleavedLength = interleaver.GetInterleavedLength();

        // --- Вывод параметров (всегда) ---
        Console.WriteLine($"Общее сообщение: {TOTAL_MESSAGE_BYTES} байт ({totalMessageBits} бит)");
        Console.WriteLine($"Информационный блок (k): {K_INFO_BITS} бит");
        Console.WriteLine($"Итеративный код: k1={K1}, k2={K2}, Паритетных групп={NUM_PARITY_GROUPS}");
        Console.WriteLine($"  -> Длина кодового слова блока (n): {n_encodedBlockLength} бит (k={coder.K}, r={coder.R})");
        Console.WriteLine($"Количество блоков: {numInfoBlocks}");
        Console.WriteLine($"Общая длина закодированных данных: {totalEncodedBits} бит");
        Console.WriteLine($"Перемежитель: {INTERLEAVER_COLUMNS} столбцов");
        Console.WriteLine($"  -> Длина данных после перемежения: {interleavedLength} бит");
        Console.WriteLine($"Количество испытаний на пакет: {NUM_TRIALS}");
        Console.WriteLine("---------------------------\n");

        // --- Основной Цикл Эксперимента ---
        foreach (int burstLength in BURST_LENGTHS)
        {
            Console.WriteLine($"\n===== Анализ для Пакета Ошибок Длиной: {burstLength} бит =====");
            int successCountWithoutInterleaving = 0; int successCountWithInterleaving = 0;

            for (int trial = 0; trial < NUM_TRIALS; trial++)
            {
                // --- Флаг для вывода только первого испытания ---
                bool showDetails = DETAILED_OUTPUT && trial == 0;

                if (showDetails) Console.WriteLine($"\n~~~~~~~~~~~~~~ Начало Испытания {trial + 1} (Детальный вывод) ~~~~~~~~~~~~~~");

                // 1. Генерация
                int[] originalMessage = GenerateRandomBinaryWord(totalMessageBits);
                if (showDetails) PrintUtils.PrintVector(originalMessage.ToArray(), "Исходное сообщение");

                // 2. Сегментация
                List<int[]> infoBlocks = SegmentData(originalMessage, K_INFO_BITS);

                // 3. Кодирование
                List<int[]> originalEncodedBlocks = new List<int[]>();
                if (showDetails) Console.WriteLine("\n--- НАЧАЛО КОДИРОВАНИЯ ВСЕХ БЛОКОВ ---");
                for (int blockIdx = 0; blockIdx < infoBlocks.Count; blockIdx++)
                {

                    bool showEncodingDetails = showDetails;

                    if (showEncodingDetails)
                    {
                        Console.WriteLine($"\n--- Кодирование блока {blockIdx + 1}/{infoBlocks.Count} ---");
                    }
                    originalEncodedBlocks.Add(coder.Encode(infoBlocks[blockIdx], showEncodingDetails));
                }
                if (showDetails) Console.WriteLine("\n--- КОНЕЦ КОДИРОВАНИЯ ВСЕХ БЛОКОВ ---");

                // 4. Конкатенация -> Xn
                int[] fullOriginalEncodedData = ConcatenateData(originalEncodedBlocks);
                if (fullOriginalEncodedData.Length != totalEncodedBits) { Console.WriteLine($"ОШИБКА КОДИРОВАНИЯ: Неожиданная длина {fullOriginalEncodedData.Length} != {totalEncodedBits}"); continue; }
                if (showDetails) PrintUtils.PrintVector(fullOriginalEncodedData.ToArray(), "Полное кодовое слово Xn"); // 6*n


                // --- Сценарий 1: Без Перемежения ---
                if (showDetails) Console.WriteLine("\n--- СЦЕНАРИЙ 1: БЕЗ ПЕРЕМЕЖЕНИЯ ---");
                try
                {
                    // 5a. Ошибка -> Yn
                    int maxErrorStartPosWithout = totalEncodedBits - 1; if (maxErrorStartPosWithout < 0) maxErrorStartPosWithout = 0;
                    int errorStartPosWithout = (maxErrorStartPosWithout >= 0) ? GlobalRandom.Next(maxErrorStartPosWithout + 1) : 0; // Handle case where totalEncodedBits = 0

                    if (showDetails) Console.WriteLine($"Внесение пакетной ошибки ({burstLength} бит) с позиции {errorStartPosWithout}");
                    int[] encodedDataWithError = coder.IntroduceBurstError(fullOriginalEncodedData, burstLength, errorStartPosWithout); // Yn
                    if (showDetails && totalEncodedBits > 0)
                    {
                        // Вывод фрагмента с ошибкой
                        int printStart = Math.Max(0, errorStartPosWithout - 10);
                        int printEnd = Math.Min(totalEncodedBits, errorStartPosWithout + burstLength + 10);
                        PrintUtils.PrintVector(fullOriginalEncodedData.Skip(printStart).Take(printEnd - printStart).ToArray(), $"  Оригинал Xn (бит {printStart}-{printEnd - 1})");
                        PrintUtils.PrintVector(encodedDataWithError.Skip(printStart).Take(printEnd - printStart).ToArray(), $"  С ошибкой Yn (бит {printStart}-{printEnd - 1})");
                    }

                    // 6a. Сегментация Yn
                    List<int[]> receivedBlocksWithout = SegmentData(encodedDataWithError, n_encodedBlockLength);

                    // 7a. Декодирование -> Yn' (блоками)
                    List<int[]> correctedBlocksWithout = new List<int[]>(); bool decodeErrorWithout = false;
                    for (int blockIdx = 0; blockIdx < receivedBlocksWithout.Count; blockIdx++)
                    {
                        int[] receivedBlock = receivedBlocksWithout[blockIdx];
                        if (receivedBlock.Length != n_encodedBlockLength) { Console.WriteLine($"ОШИБКА СЕГМЕНТАЦИИ (Без П.): Блок {receivedBlock.Length} != {n_encodedBlockLength}"); decodeErrorWithout = true; break; }

                        // Показываем декодирование только если включен детальный режим
                        bool showDecodingDetails = showDetails;
                        if (showDetails) Console.WriteLine($"\n--- Декодирование блока {blockIdx + 1}/{receivedBlocksWithout.Count} (Без перемежения) ---");
                        try
                        {
                            int[] correctedWord = coder.Decode(receivedBlock, showDecodingDetails); // verbose = showDetails
                            if (correctedWord == null || correctedWord.Length != n_encodedBlockLength) { Console.WriteLine($"ОШИБКА ДЕКОДИРОВАНИЯ (Без П.): Decode вернул null или неверную длину."); decodeErrorWithout = true; break; }
                            correctedBlocksWithout.Add(correctedWord);
                        }
                        catch (Exception ex) { Console.WriteLine($"ИСКЛЮЧЕНИЕ при декодировании (Без П.): {ex.Message}"); decodeErrorWithout = true; break; }
                    }

                    // 8a. Сборка Yn' и Сравнение Xn и Yn'
                    if (!decodeErrorWithout)
                    {
                        int[] fullCorrectedDataWithout = ConcatenateData(correctedBlocksWithout);
                        bool success = fullOriginalEncodedData.SequenceEqual(fullCorrectedDataWithout);
                        if (showDetails)
                        {
                            Console.WriteLine("\n--- Сравнение результата (Без перемежения) ---");
                            PrintUtils.PrintVector(fullOriginalEncodedData.ToArray(), "Исходное Xn (начало)");
                            PrintUtils.PrintVector(fullCorrectedDataWithout.ToArray(), "Исправленное Yn' (начало)");
                            Console.WriteLine($"Результат: {(success ? "УСПЕХ" : "НЕУДАЧА")}");
                        }
                        if (success) successCountWithoutInterleaving++;
                    }
                    else if (showDetails)
                    {
                        Console.WriteLine("\n--- Ошибка декодирования/сегментации (Без перемежения) ---");
                    }
                }
                catch (Exception ex) { Console.WriteLine($"Критическая ошибка в сценарии БЕЗ перемежения (Trial {trial + 1}): {ex.GetType().Name} - {ex.Message}"); }

                // --- Сценарий 2: С Перемежением ---
                if (showDetails) Console.WriteLine("\n\n--- СЦЕНАРИЙ 2: С ПЕРЕМЕЖЕНИЕМ ---");
                try
                {
                    // 5b. Перемежение Xn
                    int[] interleavedData = interleaver.Interleave(fullOriginalEncodedData, showDetails); // verbose = showDetails

                    // 6b. Ошибка в перемеж. -> Yn_interleaved
                    int maxErrorStartPosWith = interleavedLength - 1; if (maxErrorStartPosWith < 0) maxErrorStartPosWith = 0;
                    int errorStartPosWith = (maxErrorStartPosWith >= 0) ? GlobalRandom.Next(maxErrorStartPosWith + 1) : 0;

                    if (showDetails) Console.WriteLine($"\nВнесение пакетной ошибки ({burstLength} бит) с позиции {errorStartPosWith} В ПЕРЕМЕЖЕННЫЕ ДАННЫЕ");
                    int[] interleavedDataWithError = (int[])interleavedData.Clone();
                    int currentBurstLength = 0; // Track actual errors applied
                    for (int i = 0; i < burstLength; ++i)
                    {
                        int errPos = errorStartPosWith + i;
                        if (errPos < interleavedLength)
                        { // Убедимся, что не выходим за границы
                            interleavedDataWithError[errPos] = 1 - interleavedDataWithError[errPos];
                            currentBurstLength++;
                        }
                        else break;
                    }
                    if (showDetails && interleavedLength > 0)
                    {
                        int printStart = Math.Max(0, errorStartPosWith - 10);
                        int printEnd = Math.Min(interleavedLength, errorStartPosWith + currentBurstLength + 10);
                        PrintUtils.PrintVector(interleavedData.Skip(printStart).Take(printEnd - printStart).ToArray(), $"  Ориг. перемеж. (бит {printStart}-{printEnd - 1})");
                        PrintUtils.PrintVector(interleavedDataWithError.Skip(printStart).Take(printEnd - printStart).ToArray(), $"  С ошибкой перемеж. (бит {printStart}-{printEnd - 1})");
                    }


                    // 7b. Деперемежение -> Yn (с размаз. ошибками)
                    int[] deinterleavedData = interleaver.Deinterleave(interleavedDataWithError, showDetails); // verbose = showDetails
                    if (deinterleavedData.Length != totalEncodedBits) { Console.WriteLine($"ОШИБКА ДЕПЕРЕМЕЖЕНИЯ: Неожиданная длина {deinterleavedData.Length} != {totalEncodedBits}"); continue; }

                    // 8b. Сегментация Yn
                    List<int[]> receivedBlocksWith = SegmentData(deinterleavedData, n_encodedBlockLength);

                    // 9b. Декодирование -> Yn' (блоками)
                    List<int[]> correctedBlocksWith = new List<int[]>(); bool decodeErrorWith = false;
                    for (int blockIdx = 0; blockIdx < receivedBlocksWith.Count; blockIdx++)
                    {
                        int[] receivedBlock = receivedBlocksWith[blockIdx];
                        if (receivedBlock.Length != n_encodedBlockLength) { Console.WriteLine($"ОШИБКА СЕГМЕНТАЦИИ (С П.): Блок {receivedBlock.Length} != {n_encodedBlockLength}"); decodeErrorWith = true; break; }

                        // Показываем декодирование
                        bool showDecodingDetails = showDetails;
                        if (showDetails) Console.WriteLine($"\n--- Декодирование блока {blockIdx + 1}/{receivedBlocksWith.Count} (С перемежением) ---");
                        try
                        {
                            int[] correctedWord = coder.Decode(receivedBlock, showDecodingDetails); // verbose = showDetails
                            if (correctedWord == null || correctedWord.Length != n_encodedBlockLength) { Console.WriteLine($"ОШИБКА ДЕКОДИРОВАНИЯ (C П.): Decode вернул null или неверную длину."); decodeErrorWith = true; break; }
                            correctedBlocksWith.Add(correctedWord);
                        }
                        catch (Exception ex) { Console.WriteLine($"ИСКЛЮЧЕНИЕ при декодировании (С П.): {ex.Message}"); decodeErrorWith = true; break; }
                    }

                    // 10b. Сборка Yn' и Сравнение Xn и Yn'
                    if (!decodeErrorWith)
                    {
                        int[] fullCorrectedDataWith = ConcatenateData(correctedBlocksWith);
                        bool success = fullOriginalEncodedData.SequenceEqual(fullCorrectedDataWith);
                        if (showDetails)
                        {
                            Console.WriteLine("\n--- Сравнение результата (С перемежением) ---");
                            PrintUtils.PrintVector(fullOriginalEncodedData.Take(Math.Min(66, fullOriginalEncodedData.Length)).ToArray(), "Исходное Xn (начало)");
                            PrintUtils.PrintVector(fullCorrectedDataWith.Take(Math.Min(66, fullCorrectedDataWith.Length)).ToArray(), "Исправленное Yn' (начало)");
                            Console.WriteLine($"Результат: {(success ? "УСПЕХ" : "НЕУДАЧА")}");
                        }
                        if (success) successCountWithInterleaving++;
                    }
                    else if (showDetails)
                    {
                        Console.WriteLine("\n--- Ошибка декодирования/сегментации (С перемежением) ---");
                    }
                }
                catch (Exception ex) { Console.WriteLine($"Критическая ошибка в сценарии C перемежением (Trial {trial + 1}): {ex.GetType().Name} - {ex.Message}"); }

                if (showDetails) Console.WriteLine($"\n~~~~~~~~~~~~~~ Конец Испытания {trial + 1} ~~~~~~~~~~~~~~");

            } // Конец цикла испытаний (trial)

            // --- Вывод Результатов (всегда) ---
            double successRateWithout = (NUM_TRIALS > 0) ? (double)successCountWithoutInterleaving / NUM_TRIALS : 0.0;
            double successRateWith = (NUM_TRIALS > 0) ? (double)successCountWithInterleaving / NUM_TRIALS : 0.0;
            Console.WriteLine($"Результаты для {NUM_TRIALS} испытаний (пакет {burstLength} бит):");
            Console.WriteLine($"  Без перемежения: {successCountWithoutInterleaving} успехов ({successRateWithout:P2})");
            Console.WriteLine($"  С перемежением:  {successCountWithInterleaving} успехов ({successRateWith:P2})");
            Console.WriteLine($"--------------------------------------------------");

        } // Конец цикла по длинам пакетов ошибок

        Console.WriteLine("\nАнализ завершен.");
    }
}