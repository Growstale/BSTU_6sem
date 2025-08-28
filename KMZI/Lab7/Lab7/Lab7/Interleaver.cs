using System;
using System.Collections.Generic;
using System.Linq;

namespace Lab7 { 
public class Interleaver
{
    public int Columns { get; } // Количество столбцов в матрице перемежения
    private int _rows;          // Количество строк (вычисляется)
    private int _originalLength; // Исходная длина данных до возможного дополнения
    private int _paddedLength;   // Длина данных после дополнения до размера матрицы

    /// <summary>
    /// Инициализирует перемежитель.
    /// </summary>
    /// <param name="columns">Количество столбцов в матрице перемежения.</param>
    /// <param name="dataLength">Ожидаемая длина входных данных.</param>
    public Interleaver(int columns, int dataLength)
    {
        if (columns <= 0)
            throw new ArgumentOutOfRangeException(nameof(columns), "Количество столбцов должно быть положительным.");
        if (dataLength <= 0)
            throw new ArgumentOutOfRangeException(nameof(dataLength), "Длина данных должна быть положительной.");

        Columns = columns;
        _originalLength = dataLength;
        _rows = (int)Math.Ceiling((double)dataLength / Columns); // Вычисляем необходимое количество строк
        _paddedLength = _rows * Columns;                         // Полный размер матрицы
    }

    /// <summary>
    /// Выполняет перемежение входной последовательности битов.
    /// Данные записываются в матрицу по строкам, считываются по столбцам.
    /// При необходимости последовательность дополняется нулями справа.
    /// </summary>
    /// <param name="data">Входная последовательность битов.</param>
    /// <returns>Перемеженная последовательность битов (возможно, дополненная).</returns>
    public int[] Interleave(int[] data)
    {
        if (data.Length != _originalLength)
            throw new ArgumentException($"Ожидалась длина данных {_originalLength}, получено {data.Length}", nameof(data));

        // 1. Создаем дополненную последовательность
        int[] paddedData = new int[_paddedLength];
        Array.Copy(data, paddedData, data.Length);
        // Оставшиеся элементы уже инициализированы нулями по умолчанию

        // 2. Выполняем перемежение (читаем по столбцам)
        int[] interleavedData = new int[_paddedLength];
        int writeIndex = 0;
        for (int j = 0; j < Columns; j++) // Итерация по столбцам
        {
            for (int i = 0; i < _rows; i++) // Итерация по строкам
            {
                int readIndex = i * Columns + j; // Индекс в матрице (заполненной по строкам)
                if (readIndex < _paddedLength) // Проверка на всякий случай
                {
                    interleavedData[writeIndex++] = paddedData[readIndex];
                }
            }
        }
        return interleavedData;
    }

    /// <summary>
    /// Выполняет деперемежение входной последовательности битов.
    /// Данные записываются в матрицу по столбцам, считываются по строкам.
    /// Удаляет биты дополнения, если они были добавлены.
    /// </summary>
    /// <param name="interleavedData">Перемеженная последовательность битов (возможно, дополненная).</param>
    /// <returns>Исходная (деперемеженная) последовательность битов.</returns>
    public int[] Deinterleave(int[] interleavedData)
    {
        if (interleavedData.Length != _paddedLength)
            throw new ArgumentException($"Ожидалась длина перемеженных данных {_paddedLength}, получено {interleavedData.Length}", nameof(interleavedData));

        // 1. Выполняем деперемежение (пишем по столбцам, читаем по строкам)
        int[] paddedData = new int[_paddedLength];
        int readIndex = 0;
        for (int j = 0; j < Columns; j++) // Итерация по столбцам (куда пишем)
        {
            for (int i = 0; i < _rows; i++) // Итерация по строкам (куда пишем)
            {
                int writeIndex = i * Columns + j; // Индекс в матрице (читаемой по строкам)
                if (readIndex < _paddedLength) // Проверка на всякий случай
                {
                    paddedData[writeIndex] = interleavedData[readIndex++];
                }
            }
        }

        // 2. Удаляем дополнение
        int[] originalData = new int[_originalLength];
        Array.Copy(paddedData, originalData, _originalLength);

        return originalData;
    }

    /// <summary>
    /// Вносит пакет ошибок заданной длины в случайное место последовательности.
    /// </summary>
    /// <param name="data">Последовательность битов для внесения ошибок.</param>
    /// <param name="burstLength">Длина пакета ошибок (количество битов для инвертирования).</param>
    /// <param name="random">Генератор случайных чисел.</param>
    /// <returns>Последовательность с внесенным пакетом ошибок.</returns>
    public static int[] IntroduceBurstError(int[] data, int burstLength, Random random)
    {
        if (burstLength <= 0) return (int[])data.Clone(); // Нет ошибок
        if (burstLength > data.Length) burstLength = data.Length; // Ошибка не может быть длиннее данных

        int[] corruptedData = (int[])data.Clone();

        // Выбираем случайную начальную позицию для пакета ошибок
        int startPosition = random.Next(0, data.Length - burstLength + 1);

        // Инвертируем биты в пакете
        for (int i = 0; i < burstLength; i++)
        {
            int index = startPosition + i;
            corruptedData[index] = 1 - corruptedData[index]; // Инвертируем бит (0->1, 1->0)
        }

        Console.WriteLine($"   -> Внесен пакет ошибок длины {burstLength} с позиции {startPosition}");
        return corruptedData;
    }
}
}