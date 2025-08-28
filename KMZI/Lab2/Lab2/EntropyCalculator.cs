using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OfficeOpenXml;

namespace Lab2
{
    class EntropyCalculator
    {
        public static double CalculateInformationAmount(string text, double entropy, char[] alphabet)
        {
            text = new string(text.ToLower().Where(c => alphabet.Contains(c)).ToArray());
            return entropy * text.Length;
        }
        public static double CalculateEntropy(string text, char[] alphabet, string filePath)
        {
            text = new string(text.ToLower().Where(c => alphabet.Contains(c)).ToArray());
            int textLength = text.Length;

            if (textLength < 100)
            {
                Console.WriteLine("Текст слишком маленький для того, чтобы рассчитать энтропию");
                return 0;
            }

            var frequency = new Dictionary<char, int>();
            foreach (var letter in alphabet) frequency[letter] = 0;
            foreach (var c in text) frequency[c]++;

            Console.WriteLine("Частота появления символов:");
            foreach (var kvp in frequency)
            {
                double probability = (double)kvp.Value / textLength;
                Console.WriteLine($"Символ: '{kvp.Key}' Частота: {kvp.Value}, Вероятность: {probability:F4}");
            }

            //SaveToExcel(frequency, textLength, filePath);

            double entropy = 0;
            foreach (var kvp in frequency)
            {
                double probability = (double)kvp.Value / textLength;
                if (probability > 0) entropy += probability * Math.Log2(probability);
            }

            return -entropy;
        }
        public static double EffectiveEntropy(double p)
        {
            if (p == 0 || p == 1)
            {
                return 0;
            }

            double q = 1 - p;
            return -p * Math.Log2(p) - q * Math.Log2(q);
        }

        private static void SaveToExcel(Dictionary<char, int> frequency, int textLength, string filePath)
        {
            using (var package = new ExcelPackage())
            {
                var worksheet = package.Workbook.Worksheets.Add("Frequency Data");

                worksheet.Cells[1, 1].Value = "Символ";
                worksheet.Cells[1, 2].Value = "Частота";
                worksheet.Cells[1, 3].Value = "Вероятность";

                int row = 2;
                foreach (var kvp in frequency)
                {
                    double probability = (double)kvp.Value / textLength;
                    worksheet.Cells[row, 1].Value = kvp.Key;
                    worksheet.Cells[row, 2].Value = kvp.Value;
                    worksheet.Cells[row, 3].Value = probability;
                    row++;
                }

                package.SaveAs(new System.IO.FileInfo(filePath));
            }
        }

    }
}
