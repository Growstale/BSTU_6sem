using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

public static class PrintUtils // Вспомогательный класс для вывода
{
    public static string FormatBinaryVector(IEnumerable<int> vector)
    {
        return string.Join("", vector);
    }

    public static void PrintVector(IEnumerable<int> vector, string label)
    {
        Console.WriteLine($"{label}: [{FormatBinaryVector(vector)}]");
    }

    public static void PrintMatrix(int[,] matrix, string label)
    {
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
        Console.WriteLine($"{label}:");
        int dim1 = matrix.GetLength(0); // k1
        int dim2 = matrix.GetLength(1); // k2
        int dim3 = matrix.GetLength(2); // z
        for (int k = 0; k < dim3; k++)
        {
            Console.WriteLine($"  Layer {k}:");
            for (int i = 0; i < dim1; i++)
            {
                Console.Write("    [");
                for (int j = 0; j < dim2; j++)
                {
                    Console.Write(matrix[i, j, k]);
                    if (j < dim2 - 1) Console.Write(", ");
                }
                Console.WriteLine("]");
            }
        }
    }
}

public class IterativeCode // Это главный класс, реализующий логику итерационного кодирования/декодирования
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
    private int[] _receivedWordYn;    // Yn
    private int[] _correctedWordYnPrime; // Yn после исправления

    private int[,] _matrix2D;
    private int[,,] _matrix3D;

    private int[] _parityGroup1; // Row parity
    private int[] _parityGroup2; // Column parity
    private int[] _parityGroup3; // Depth parity (for 3D)
    private int[] _parityGroup4; // Overall parity

    private readonly Random _random = new Random();
    private const int MAX_DECODING_ITERATIONS = 10; // Константа, ограничивающая максимальное число итераций в процессе декодирования,
                                                    // чтобы избежать бесконечных циклов

    public IterativeCode(int k1, int k2, int? z, int numParityGroups)
    {
        K1 = k1;
        K2 = k2;
        Z = z;
        NumParityGroups = numParityGroups;

        if (Z.HasValue)
        {
            K = K1 * K2 * Z.Value;
            _matrix3D = new int[K1, K2, Z.Value];
        }
        else
        {
            K = K1 * K2;
            _matrix2D = new int[K1, K2];
        }
        // Вычисляет количество проверочных битов R
        CalculateRedundancy();
        N = K + R;
    }

    private void CalculateRedundancy()
    // Основная задача этого метода — вычислить общее количество проверочных (избыточных) битов
    {

        R = 0;
        if (!Z.HasValue)
        {
            if (NumParityGroups >= 1) R += K1; // по одному биту четности на каждую строку
            if (NumParityGroups >= 2) R += K2; // по одному биту четности на каждый столбец
        }
        else
        {
            // As per assumption for variant 3: 4 groups = k1-dir, k2-dir, z-dir, overall
            if (NumParityGroups >= 1) R += K2 * Z.Value; // проверки вдоль K1 для каждой комбинации (k2, z)
            if (NumParityGroups >= 2) R += K1 * Z.Value; // проверки вдоль K2 для каждой комбинации (k1, z)
            if (NumParityGroups >= 3) R += K1 * K2;     // проверки вдоль Z для каждой комбинации (k1, k2)
            if (NumParityGroups >= 4) R += 1;         // общая проверка четности для информационных битов
        }
    }


    // --- Этапы Кодирования ---

    public int[] Encode(int[] informationWord)
    {
        if (informationWord == null || informationWord.Length != K)
        {
            throw new ArgumentException($"Information word must have length {K}.");
        }
        _informationWord = (int[])informationWord.Clone();

        // 1. для размещения информационных битов в матрице
        FillMatrix(_informationWord);

        // 2. для вычисления всех проверочных битов
        CalculateParityBits();

        // 3. для сборки финального кодового слова Xn
        FormCodewordXn();

        return (int[])_codewordXn.Clone();
    }

    private void FillMatrix(int[] data)
    {
        int index = 0;
        if (_matrix2D != null)
        {
            for (int i = 0; i < K1; i++)
            {
                for (int j = 0; j < K2; j++)
                {
                    _matrix2D[i, j] = data[index++];
                }
            }
        }
        else // 3D
        {
            for (int k = 0; k < Z.Value; k++) // Layer (z)
            {
                for (int i = 0; i < K1; i++)  // Row (k1)
                {
                    for (int j = 0; j < K2; j++) // Column (k2)
                    {
                        _matrix3D[i, j, k] = data[index++];
                    }
                }
            }
        }
    }

    private int CalculateParity(IEnumerable<int> bits) // Вспомогательный метод для вычисления бита четности (по модулю 2)
    {
        return bits.Aggregate(0, (acc, bit) => acc ^ bit); // XOR sum
    }

    private void CalculateParityBits()
    {
        if (_matrix2D != null) // 2D Case
        {
            // Group 1: проверка по строкам
            _parityGroup1 = new int[K1];
            for (int i = 0; i < K1; i++)
            {
                int[] row = Enumerable.Range(0, K2).Select(j => _matrix2D[i, j]).ToArray();
                _parityGroup1[i] = CalculateParity(row);
            }

            // Group 2: проверка по столбцам
            _parityGroup2 = new int[K2];
            for (int j = 0; j < K2; j++)
            {
                int[] col = Enumerable.Range(0, K1).Select(i => _matrix2D[i, j]).ToArray();
                _parityGroup2[j] = CalculateParity(col);
            }
            _parityGroup3 = null; // не используются
            _parityGroup4 = null; // не используются
        }
        else // 3D Case
        {
            // Group 1: Parity along k1 (fixing k2, z)
            _parityGroup1 = new int[K2 * Z.Value];
            int p1Idx = 0;
            for (int k = 0; k < Z.Value; k++)
            {
                for (int j = 0; j < K2; j++)
                {
                    _parityGroup1[p1Idx++] = CalculateParity(Enumerable.Range(0, K1).Select(i => _matrix3D[i, j, k]));
                }
            }

            // Group 2: Parity along k2 (fixing k1, z)
            _parityGroup2 = new int[K1 * Z.Value];
            int p2Idx = 0;
            for (int k = 0; k < Z.Value; k++)
            {
                for (int i = 0; i < K1; i++)
                {
                    _parityGroup2[p2Idx++] = CalculateParity(Enumerable.Range(0, K2).Select(j => _matrix3D[i, j, k]));
                }
            }

            // Group 3: Parity along z (fixing k1, k2)
            _parityGroup3 = new int[K1 * K2];
            int p3Idx = 0;
            for (int i = 0; i < K1; i++)
            {
                for (int j = 0; j < K2; j++)
                {
                    _parityGroup3[p3Idx++] = CalculateParity(Enumerable.Range(0, Z.Value).Select(k => _matrix3D[i, j, k]));
                }
            }

            // Group 4: Overall parity of information bits
            _parityGroup4 = new int[1];
            _parityGroup4[0] = CalculateParity(_informationWord);
        }
    }

    private void FormCodewordXn() // Формирует полное кодовое слово
    {
        var codewordList = new List<int>(_informationWord);
        if (_parityGroup1 != null) codewordList.AddRange(_parityGroup1);
        if (_parityGroup2 != null) codewordList.AddRange(_parityGroup2);
        if (_parityGroup3 != null) codewordList.AddRange(_parityGroup3);
        if (_parityGroup4 != null) codewordList.AddRange(_parityGroup4);
        _codewordXn = codewordList.ToArray();

        // Сравнивает фактическую длину _codewordXn.Length с ожидаемой N
        if (_codewordXn.Length != N)
        {
            Console.WriteLine($"Warning: Calculated codeword length ({_codewordXn.Length}) doesn't match expected N ({N}). R={R}");
            N = _codewordXn.Length;
        }
    }

    // --- Генерация Ошибок ---

    public int[] IntroduceErrors(int errorCount) // Вносит заданное количество (errorCount) случайных ошибок в _codewordXn
    {
        if (_codewordXn == null)
        {
            throw new InvalidOperationException("Encoding must be performed before introducing errors.");
        }
        if (errorCount < 0) errorCount = 0;
        if (errorCount > N) errorCount = N;


        _receivedWordYn = (int[])_codewordXn.Clone(); 

        if (errorCount == 0) return (int[])_receivedWordYn.Clone();


        var indicesToFlip = new HashSet<int>();
        while (indicesToFlip.Count < errorCount)
        {
            indicesToFlip.Add(_random.Next(N)); 
        }

        foreach (int index in indicesToFlip)
        {
            _receivedWordYn[index] = 1 - _receivedWordYn[index];
        }

        return (int[])_receivedWordYn.Clone();
    }

    // --- Этапы Декодирования ---

    public int[] Decode(bool verboseOutput = false)
    {
        if (_receivedWordYn == null)
        {
            throw new InvalidOperationException("Errors must be introduced (or Yn set) before decoding.");
        }

        _correctedWordYnPrime = (int[])_receivedWordYn.Clone();

        int[,] currentMatrix2D = null;
        int[,,] currentMatrix3D = null;

        // Извлечение данных
        int[] currentData = _correctedWordYnPrime.Take(K).ToArray(); // Это Yk (возможно, с ошибками)
        int[] receivedP1 = _correctedWordYnPrime.Skip(K).Take(_parityGroup1?.Length ?? 0).ToArray(); // Это Yr (Received) - Группа 1
        int[] receivedP2 = _correctedWordYnPrime.Skip(K + (_parityGroup1?.Length ?? 0)).Take(_parityGroup2?.Length ?? 0).ToArray(); // Yr (Received) - Группа 2
        int[] receivedP3 = _correctedWordYnPrime.Skip(K + (_parityGroup1?.Length ?? 0) + (_parityGroup2?.Length ?? 0)).Take(_parityGroup3?.Length ?? 0).ToArray(); // Yr (Received) - Группа 3
        int[] receivedP4 = _correctedWordYnPrime.Skip(K + (_parityGroup1?.Length ?? 0) + (_parityGroup2?.Length ?? 0) + (_parityGroup3?.Length ?? 0)).Take(_parityGroup4?.Length ?? 0).ToArray(); // Yr (Received) - Группа 4

        // Собираем полное "Yr (Received)" для вывода
        var fullReceivedYrList = new List<int>();
        if (receivedP1 != null) fullReceivedYrList.AddRange(receivedP1);
        if (receivedP2 != null) fullReceivedYrList.AddRange(receivedP2);
        if (receivedP3 != null) fullReceivedYrList.AddRange(receivedP3);
        if (receivedP4 != null) fullReceivedYrList.AddRange(receivedP4);

        // Инициализация рабочей матрицы НЕИСПРАВЛЕННЫМИ данными currentData (Yk)
        if (!Z.HasValue)
        {
            currentMatrix2D = new int[K1, K2];
            FillMatrix2DFromData(currentData, currentMatrix2D);
        }
        else
        {
            currentMatrix3D = new int[K1, K2, Z.Value];
            FillMatrix3DFromData(currentData, currentMatrix3D);
        }

        // Вычисление и вывод Yr по НЕИСПРАВЛЕННЫМ данным (Yk) ДО начала итераций
        if (verboseOutput)
        {
            Console.WriteLine("\n------");
            PrintUtils.PrintVector(fullReceivedYrList, "Yr "); // Выводим полученное Yr
        }

        // Вычисляем, каким ДОЛЖЕН быть Yr, если бы данные были currentData (Yk)
        var calculatedYrFromYkList = new List<int>();
        // Используем те же методы, что и в CalculateParityBits, но на текущей матрице
        if (!Z.HasValue) // 2D
        {
            if (NumParityGroups >= 1)
            {
                var tempP1 = new int[K1];
                for (int i = 0; i < K1; i++) tempP1[i] = CalculateParity(Enumerable.Range(0, K2).Select(j => currentMatrix2D[i, j]));
                calculatedYrFromYkList.AddRange(tempP1);
            }
            if (NumParityGroups >= 2)
            {
                var tempP2 = new int[K2];
                for (int j = 0; j < K2; j++) tempP2[j] = CalculateParity(Enumerable.Range(0, K1).Select(i => currentMatrix2D[i, j]));
                calculatedYrFromYkList.AddRange(tempP2);
            }
        }
        else // 3D
        {
            if (NumParityGroups >= 1)
            {
                var tempP1 = new int[K2 * Z.Value];
                int p1Idx = 0; for (int k = 0; k < Z.Value; k++) for (int j = 0; j < K2; j++) tempP1[p1Idx++] = CalculateParity(Enumerable.Range(0, K1).Select(i => currentMatrix3D[i, j, k]));
                calculatedYrFromYkList.AddRange(tempP1);
            }
            if (NumParityGroups >= 2)
            {
                var tempP2 = new int[K1 * Z.Value];
                int p2Idx = 0; for (int k = 0; k < Z.Value; k++) for (int i = 0; i < K1; i++) tempP2[p2Idx++] = CalculateParity(Enumerable.Range(0, K2).Select(j => currentMatrix3D[i, j, k]));
                calculatedYrFromYkList.AddRange(tempP2);
            }
            if (NumParityGroups >= 3)
            {
                var tempP3 = new int[K1 * K2];
                int p3Idx = 0; for (int i = 0; i < K1; i++) for (int j = 0; j < K2; j++) tempP3[p3Idx++] = CalculateParity(Enumerable.Range(0, Z.Value).Select(k => currentMatrix3D[i, j, k]));
                calculatedYrFromYkList.AddRange(tempP3);
            }
            if (NumParityGroups >= 4)
            {
                var tempP4 = new int[1];
                tempP4[0] = CalculateParity(currentData); // Используем извлеченные данные Yk
                calculatedYrFromYkList.AddRange(tempP4);
            }
        }
        if (verboseOutput)
            PrintUtils.PrintVector(calculatedYrFromYkList, "Yr'"); // Выводим Yr, вычисленное по Yk

        // Сравнение покажет разницу (это и есть синдром в развернутом виде)
        bool initialMatch = fullReceivedYrList.SequenceEqual(calculatedYrFromYkList);
        if (verboseOutput)
        {
            Console.WriteLine($"Match: {initialMatch}");
            Console.WriteLine("------\n");
        }

        // Итеративный Цикл Декодирования
        for (int iter = 0; iter < MAX_DECODING_ITERATIONS; iter++)
        {

            bool correctionMade = false;

            // --- Вычисление Синдромов ---
            int[] syndrome1 = null, syndrome2 = null, syndrome3 = null, syndrome4 = null;
            if (!Z.HasValue) // 2D Case
            {
                syndrome1 = CalculateSyndrome2D(currentMatrix2D, receivedP1, 1); // Row Syndrome
                syndrome2 = CalculateSyndrome2D(currentMatrix2D, receivedP2, 2); // Col Syndrome
            }
            else // 3D Case
            {
                syndrome1 = CalculateSyndrome3D(currentMatrix3D, receivedP1, 1); // k1 dir
                syndrome2 = CalculateSyndrome3D(currentMatrix3D, receivedP2, 2); // k2 dir
                syndrome3 = CalculateSyndrome3D(currentMatrix3D, receivedP3, 3); // z dir
                syndrome4 = CalculateSyndrome3D(currentMatrix3D, receivedP4, 4); // overall
            }

            // Проверка Сходимости
            bool allZero = (syndrome1?.All(s => s == 0) ?? true) &&
                           (syndrome2?.All(s => s == 0) ?? true) &&
                           (syndrome3?.All(s => s == 0) ?? true) &&
                           (syndrome4?.All(s => s == 0) ?? true);

            if (allZero)
            {
                break;
            }

            // --- Идентификация и Исправление Ошибок ---
            if (!Z.HasValue && currentMatrix2D != null) // 2D Correction
            {
                for (int i = 0; i < K1; i++)
                {
                    for (int j = 0; j < K2; j++)
                    {
                        int failingChecks = 0;
                        if (syndrome1 != null && syndrome1[i] == 1) failingChecks++;
                        if (syndrome2 != null && syndrome2[j] == 1) failingChecks++;

                        if (failingChecks >= 2)
                        {
                            currentMatrix2D[i, j] = 1 - currentMatrix2D[i, j];
                            correctionMade = true;
                        }
                    }
                }
            }
            else if (Z.HasValue && currentMatrix3D != null)
            {
                for (int i = 0; i < K1; i++)
                {
                    for (int j = 0; j < K2; j++)
                    {
                        for (int k = 0; k < Z.Value; k++)
                        {
                            int failingChecks = 0;
                            if (syndrome1 != null && syndrome1[k * K2 + j] == 1) failingChecks++;
                            if (syndrome2 != null && syndrome2[k * K1 + i] == 1) failingChecks++;
                            if (syndrome3 != null && syndrome3[i * K2 + j] == 1) failingChecks++;

                            if (failingChecks >= 2)
                            {
                                currentMatrix3D[i, j, k] = 1 - currentMatrix3D[i, j, k];
                                correctionMade = true;
                            }
                        }
                    }
                }
            }

            if (!correctionMade && !allZero)
            {
                break;
            }
            if (iter == MAX_DECODING_ITERATIONS - 1 && !allZero)
            {
                // Log message about reaching max iterations
            }
        } 

        // --- Реконструкция Финального Слова Yn' ---
        if (!Z.HasValue)
        {
            CalculateParityBitsFromMatrix(currentMatrix2D); // Пересчет по ИСПРАВЛЕННОЙ матрице
            currentData = FlattenMatrix(currentMatrix2D); // Получение ИСПРАВЛЕННЫХ данных
        }
        else
        {
            CalculateParityBitsFromMatrix(currentMatrix3D); // Пересчет по ИСПРАВЛЕННОЙ матрице
            currentData = FlattenMatrix(currentMatrix3D); // Получение ИСПРАВЛЕННЫХ данных
        }

        var correctedList = new List<int>(currentData);
        if (_parityGroup1 != null) correctedList.AddRange(_parityGroup1);
        if (_parityGroup2 != null) correctedList.AddRange(_parityGroup2);
        if (_parityGroup3 != null) correctedList.AddRange(_parityGroup3);
        if (_parityGroup4 != null) correctedList.AddRange(_parityGroup4);
        _correctedWordYnPrime = correctedList.ToArray(); // Финальное Yn'

        return (int[])_correctedWordYnPrime.Clone();
    }

    // --- Helper methods for Decoding ---

    private void FillMatrix2DFromData(int[] data, int[,] matrix)
    {
        int index = 0;
        for (int i = 0; i < K1; i++)
            for (int j = 0; j < K2; j++)
                matrix[i, j] = data[index++];
    }

    private void FillMatrix3DFromData(int[] data, int[,,] matrix)
    {
        int index = 0;
        for (int k = 0; k < Z.Value; k++)
            for (int i = 0; i < K1; i++)
                for (int j = 0; j < K2; j++)
                    matrix[i, j, k] = data[index++];
    }

    private int[] FlattenMatrix(int[,] matrix)
    {
        int[] data = new int[K];
        int index = 0;
        for (int i = 0; i < K1; i++)
            for (int j = 0; j < K2; j++)
                data[index++] = matrix[i, j];
        return data;
    }

    private int[] FlattenMatrix(int[,,] matrix)
    {
        int[] data = new int[K];
        int index = 0;
        for (int k = 0; k < Z.Value; k++)
            for (int i = 0; i < K1; i++)
                for (int j = 0; j < K2; j++)
                    data[index++] = matrix[i, j, k];
        return data;
    }


    private int[] CalculateSyndrome2D(int[,] matrix, int[] receivedParity, int groupIndex)
    {
        if (receivedParity == null) return new int[0];

        int[] calculatedParity;
        int[] syndrome = new int[receivedParity.Length];

        if (groupIndex == 1) // Row Parity Syndrome
        {
            calculatedParity = new int[K1];
            for (int i = 0; i < K1; i++)
            {
                calculatedParity[i] = CalculateParity(Enumerable.Range(0, K2).Select(j => matrix[i, j]));
                syndrome[i] = calculatedParity[i] ^ receivedParity[i];
            }
        }
        else if (groupIndex == 2) // Column Parity Syndrome
        {
            calculatedParity = new int[K2];
            for (int j = 0; j < K2; j++)
            {
                calculatedParity[j] = CalculateParity(Enumerable.Range(0, K1).Select(i => matrix[i, j]));
                syndrome[j] = calculatedParity[j] ^ receivedParity[j];
            }
        }
        else
        {
            return new int[0]; // Invalid group index for 2D
        }
        return syndrome;
    }

    private int[] CalculateSyndrome3D(int[,,] matrix, int[] receivedParity, int groupIndex)
    {
        if (receivedParity == null) return new int[0];

        int[] calculatedParity;
        int[] syndrome = new int[receivedParity.Length];

        switch (groupIndex)
        {
            case 1: // k1 direction parity
                calculatedParity = new int[K2 * Z.Value];
                int p1Idx = 0;
                for (int k = 0; k < Z.Value; k++)
                {
                    for (int j = 0; j < K2; j++)
                    {
                        calculatedParity[p1Idx] = CalculateParity(Enumerable.Range(0, K1).Select(i => matrix[i, j, k]));
                        syndrome[p1Idx] = calculatedParity[p1Idx] ^ receivedParity[p1Idx];
                        p1Idx++;
                    }
                }
                break;
            case 2: // k2 direction parity
                calculatedParity = new int[K1 * Z.Value];
                int p2Idx = 0;
                for (int k = 0; k < Z.Value; k++)
                {
                    for (int i = 0; i < K1; i++)
                    {
                        calculatedParity[p2Idx] = CalculateParity(Enumerable.Range(0, K2).Select(j => matrix[i, j, k]));
                        syndrome[p2Idx] = calculatedParity[p2Idx] ^ receivedParity[p2Idx];
                        p2Idx++;
                    }
                }
                break;
            case 3: // z direction parity
                calculatedParity = new int[K1 * K2];
                int p3Idx = 0;
                for (int i = 0; i < K1; i++)
                {
                    for (int j = 0; j < K2; j++)
                    {
                        calculatedParity[p3Idx] = CalculateParity(Enumerable.Range(0, Z.Value).Select(k => matrix[i, j, k]));
                        syndrome[p3Idx] = calculatedParity[p3Idx] ^ receivedParity[p3Idx];
                        p3Idx++;
                    }
                }
                break;
            case 4: // overall info parity
                calculatedParity = new int[1];
                // Calculate parity from the *current data* in the matrix
                int[] currentData = FlattenMatrix(matrix);
                calculatedParity[0] = CalculateParity(currentData);
                syndrome[0] = calculatedParity[0] ^ receivedParity[0];
                break;
            default:
                return new int[0]; // Invalid group
        }
        return syndrome;
    }

    private void CalculateParityBitsFromMatrix(int[,] matrix)
    {
        if (NumParityGroups >= 1)
        {
            _parityGroup1 = new int[K1];
            for (int i = 0; i < K1; i++)
                _parityGroup1[i] = CalculateParity(Enumerable.Range(0, K2).Select(j => matrix[i, j]));
        }
        if (NumParityGroups >= 2)
        {
            _parityGroup2 = new int[K2];
            for (int j = 0; j < K2; j++)
                _parityGroup2[j] = CalculateParity(Enumerable.Range(0, K1).Select(i => matrix[i, j]));
        }
        _parityGroup3 = null;
        _parityGroup4 = null;
    }

    private void CalculateParityBitsFromMatrix(int[,,] matrix)
    {
        // Group 1: Parity along k1
        if (NumParityGroups >= 1)
        {
            _parityGroup1 = new int[K2 * Z.Value];
            int p1Idx = 0;
            for (int k = 0; k < Z.Value; k++)
                for (int j = 0; j < K2; j++)
                    _parityGroup1[p1Idx++] = CalculateParity(Enumerable.Range(0, K1).Select(i => matrix[i, j, k]));
        }

        // Group 2: Parity along k2
        if (NumParityGroups >= 2)
        {
            _parityGroup2 = new int[K1 * Z.Value];
            int p2Idx = 0;
            for (int k = 0; k < Z.Value; k++)
                for (int i = 0; i < K1; i++)
                    _parityGroup2[p2Idx++] = CalculateParity(Enumerable.Range(0, K2).Select(j => matrix[i, j, k]));
        }

        // Group 3: Parity along z
        if (NumParityGroups >= 3)
        {
            _parityGroup3 = new int[K1 * K2];
            int p3Idx = 0;
            for (int i = 0; i < K1; i++)
                for (int j = 0; j < K2; j++)
                    _parityGroup3[p3Idx++] = CalculateParity(Enumerable.Range(0, Z.Value).Select(k => matrix[i, j, k]));
        }

        // Group 4: Overall parity of information bits
        if (NumParityGroups >= 4)
        {
            _parityGroup4 = new int[1];
            int[] currentData = FlattenMatrix(matrix); // Get current data from corrected matrix
            _parityGroup4[0] = CalculateParity(currentData);
        }
    }

    // --- Analysis ---

    public bool AnalyzeCorrection()
    {
        if (_codewordXn == null || _correctedWordYnPrime == null)
        {
            return false;
        }
        if (_codewordXn.Length != _correctedWordYnPrime.Length)
        {
            return false;
        }
        return _codewordXn.SequenceEqual(_correctedWordYnPrime);
        // возвращает true, если все элементы в обоих массивах совпадают по значению и порядку, и false в противном случае
    }

    public int[] GetInformationWord() => (int[])_informationWord?.Clone();
    public int[] GetCodewordXn() => (int[])_codewordXn?.Clone();
    public int[] GetReceivedWordYn() => (int[])_receivedWordYn?.Clone();
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

// --- Main Program ---
class Program
{
    static readonly Random GlobalRandom = new Random();

    static int[] GenerateRandomBinaryWord(int length)
    {
        int[] word = new int[length];
        for (int i = 0; i < length; i++)
        {
            word[i] = GlobalRandom.Next(2);
        }
        return word;
    }

    static void RunSingleDemo(IterativeCode coder, int errorCount)
    // Проводит одну полную демонстрацию: генерация слова -> кодирование -> внесение ошибок -> декодирование -> анализ
    {
        Console.WriteLine($"\n===================== (Error Count = {errorCount}) =====================");

        // 1. Генерация информационного слова
        int[] infoWord = GenerateRandomBinaryWord(coder.K);
        PrintUtils.PrintVector(infoWord, "Xk");

        // 2. Encode
        int[] xn = coder.Encode(infoWord);
        if (coder.Z == null)
            PrintUtils.PrintMatrix(coder.GetMatrix2D(), "Matrix (2D)");
        else
            PrintUtils.PrintMatrix(coder.GetMatrix3D(), "Matrix (3D)");

        PrintUtils.PrintVector(coder.GetParityGroup(1), "Parity Group 1");
        PrintUtils.PrintVector(coder.GetParityGroup(2), "Parity Group 2");
        if (coder.Z != null)
        {
            PrintUtils.PrintVector(coder.GetParityGroup(3), "Parity Group 3");
            PrintUtils.PrintVector(coder.GetParityGroup(4), "Parity Group 4");
        }
        PrintUtils.PrintVector(xn, "Xn");

        // 3. Introduce Errors
        int[] yn = coder.IntroduceErrors(errorCount);
        PrintUtils.PrintVector(yn, "Yn");

        List<int> errorPositions = new List<int>();
        for (int i = 0; i < xn.Length; i++)
        {
            if (xn[i] != yn[i]) errorPositions.Add(i);
        }


        Console.WriteLine($"Error location: {string.Join(", ", errorPositions)}");


        // 4. Decode
        int[] ynPrime = coder.Decode(true); 


        PrintUtils.PrintVector(ynPrime, "Yn'");

        // 5. Analyze
        bool success = coder.AnalyzeCorrection();
        Console.WriteLine($"\nCorrection Successful: {success}");
        if (!success)
        {
            Console.WriteLine("Comparison:");
            Console.WriteLine($"Xn:  {PrintUtils.FormatBinaryVector(xn)}");
            Console.WriteLine($"Yn': {PrintUtils.FormatBinaryVector(ynPrime)}");
            List<int> remainingErrorPositions = new List<int>();
            for (int i = 0; i < xn.Length; i++)
            {
                if (xn[i] != ynPrime[i]) remainingErrorPositions.Add(i);
            }
            Console.WriteLine($"(Remaining errors at indices: {string.Join(", ", remainingErrorPositions)})");
        }
        Console.WriteLine("========================================================================");
    }

    static void RunAnalysis(IterativeCode coder, int errorMultiplicity, int numTrials)
    {
        Console.WriteLine($"\n=========== (Analysis: Error Count = {errorMultiplicity}, Tests = {numTrials}) ===========");

        if (numTrials <= 0) return;

        int correctedCount = 0;

        int[] infoWord = GenerateRandomBinaryWord(coder.K);
        int[] xn = coder.Encode(infoWord);

        for (int i = 0; i < numTrials; i++)
        {
            coder.IntroduceErrors(errorMultiplicity); // Yn
            coder.Decode(); // Yn'
            if (coder.AnalyzeCorrection()) // Compare Xn and Yn'
            {
                correctedCount++;
            }

            if ((i + 1) % (numTrials / 10 == 0 ? numTrials / 10 + 1 : numTrials / 10) == 0)
            { 
                Console.Write($"{(int)(((double)(i + 1) / numTrials) * 100)}% ");
            }
        }

        // Calculate and display results (N3/N1)
        double correctionRate = (double)correctedCount / numTrials;

        Console.WriteLine($"Tests: {numTrials}");
        Console.WriteLine($"Correctly Corrected : {correctedCount}");
        Console.WriteLine($"Correction Rate: {correctionRate:P2}");
        Console.WriteLine("=======================================================================================");
    }


    static void Main(string[] args)
    {
        Console.OutputEncoding = Encoding.UTF8;
        Console.WriteLine("1. k=24, k1=4, k2=6, 2D, Row/Col Parity");
        Console.WriteLine("2. k=24, k1=3, k2=8, 2D, Row/Col Parity");
        Console.WriteLine("3. k=24, k1=3, k2=2, z=4, 3D, 4 Parity Groups (k1, k2, z dirs + overall)");
        Console.WriteLine("4. k=24, k1=6, k2=2, z=2, 3D, 4 Parity Groups (k1, k2, z dirs + overall)");


        int k1 = 0, k2 = 0;
        int? z = null;
        int numParityGroups = 0;

        

        // 1
        k1 = 4; k2 = 6; z = null; numParityGroups = 2;
        IterativeCode coder1 = new IterativeCode(k1, k2, z, numParityGroups);
        Console.WriteLine($"K={coder1.K}, R={coder1.R}, N={coder1.N}");

        Console.ReadLine();
        RunSingleDemo(coder1, 1);
        Console.ReadLine();
        RunSingleDemo(coder1, 2);
        Console.ReadLine();
        RunSingleDemo(coder1, 3);
        Console.ReadLine();

        // 2
        k1 = 3; k2 = 8; z = null; numParityGroups = 2;
        IterativeCode coder2 = new IterativeCode(k1, k2, z, numParityGroups);
        Console.WriteLine($"K={coder2.K}, R={coder2.R}, N={coder2.N}");

        Console.ReadLine();
        RunSingleDemo(coder2, 1);
        Console.ReadLine();
        RunSingleDemo(coder2, 2);
        Console.ReadLine();
        RunSingleDemo(coder2, 3);
        Console.ReadLine();

        // 3
        k1 = 3; k2 = 2; z = 4; numParityGroups = 4;
        IterativeCode coder3 = new IterativeCode(k1, k2, z, numParityGroups);
        Console.WriteLine($"K={coder3.K}, R={coder3.R}, N={coder3.N}");

        Console.ReadLine();
        RunSingleDemo(coder3, 1);
        Console.ReadLine();
        RunSingleDemo(coder3, 2);
        Console.ReadLine();
        RunSingleDemo(coder3, 3);
        Console.ReadLine();

        // 4
        k1 = 6; k2 = 2; z = 2; numParityGroups = 4;
        IterativeCode coder4 = new IterativeCode(k1, k2, z, numParityGroups);
        Console.WriteLine($"K={coder4.K}, R={coder4.R}, N={coder4.N}");

        Console.ReadLine();
        RunSingleDemo(coder4, 1);
        Console.ReadLine();
        RunSingleDemo(coder4, 2);
        Console.ReadLine();
        RunSingleDemo(coder4, 3);
        Console.ReadLine();


        int trials = 1000;
        RunAnalysis(coder1, 1, trials);
        Console.ReadLine();
        RunAnalysis(coder1, 2, trials);
        Console.ReadLine();
        RunAnalysis(coder1, 3, trials); 
        
        Console.ReadLine();
    }
}