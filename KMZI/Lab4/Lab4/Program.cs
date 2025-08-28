using Microsoft.VisualBasic;
using System;

namespace Lab4
{
    class Program
    {
        static void Main()
        {
            string inputText = FileReader.ReadTextFromFile("inputText.txt");
            int[] Xk = FileReader.TextToBinaryArray(inputText);
            foreach (var x in Xk) Console.Write(x);
            Console.WriteLine();
            int k = Xk.Length;

            Console.WriteLine("=========================================");

            int[,]? H = Hemming.GenerateMatrix(k);

            if (H == null) 
            {
                return;
            }

            Console.WriteLine("=========================================");

            Console.WriteLine("Проверочная матрица H:");
            Hemming.PrintMatrix(H);

            Console.WriteLine("=========================================");

            int[] Xr;
            int[] Xn = Hemming.SolveHamming(H, Xk, out Xr);

            Console.WriteLine("Избыточное слово:");
            foreach (var x in Xr) Console.Write(x);
            Console.WriteLine();

            Console.WriteLine("Кодовое слово:");
            foreach (var x in Xn) Console.Write(x);
            Console.WriteLine();

            int[] Yn1 = new int [Xn.Length];
            int[] Yn2 = new int [Xn.Length];
            int[] Yn3 = new int [Xn.Length];
            Xn.CopyTo(Yn1, 0);
            Xn.CopyTo(Yn2, 0);
            Xn.CopyTo(Yn3, 0);
            Hemming.ChangeValue(Yn2);
            Hemming.ChangeValue(Yn3);
            Hemming.ChangeValue(Yn3);

            Console.WriteLine("=========================================");
            Hemming.CheckYn(Yn1, k, H);
            Console.WriteLine("=========================================");
            Hemming.CheckYn(Yn2, k, H);
            Console.WriteLine("=========================================");
            Hemming.CheckYn(Yn3, k, H);
            Console.WriteLine("=========================================");

        }
    }
}
