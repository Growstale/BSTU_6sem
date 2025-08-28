using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Lab2
{
    class Program
    {
        public static void Main()
        {
            // задание а

            Console.WriteLine("Задание a\n");

            string italianText = FileReader.ReadTextFromFile("italianText.txt");
            string mongolianText = FileReader.ReadTextFromFile("mongolianText.txt");

            char[] ItalianAlphabet = "abcdefghijklmnopqrstuvwxyzàèéìíîòóùú".ToCharArray();
            char[] MongolianAlphabet = "абвгдеёжзийклмноөпрстуүфхцчшщъыьэюя".ToCharArray();

            Console.WriteLine($"Итальянский язык");
            Console.WriteLine("==============================================");
            double italianEntropy = EntropyCalculator.CalculateEntropy(italianText, ItalianAlphabet, "frequency_data_italian.xlsx");
            Console.WriteLine($"Энтропия для итальянского текста: {italianEntropy:F4}");
            Console.WriteLine("==============================================\n");

            Console.WriteLine($"\nМонгольский язык");
            Console.WriteLine("==============================================");
            double mongolianEntropy = EntropyCalculator.CalculateEntropy(mongolianText, MongolianAlphabet, "frequency_data_mongolian.xlsx");
            Console.WriteLine($"Энтропия для монгольского текста: {mongolianEntropy:F4}");
            Console.WriteLine("==============================================\n");

            // задание б

            Console.WriteLine("Задание б\n");

            string italianTextBinary = FileReader.ReadTextFromFile("italianTextBinary.txt");
            string mongolianTextBinary = FileReader.ReadTextFromFile("mongolianTextBinary.txt");

            char[] BinaryAlphabet = "01".ToCharArray();

            Console.WriteLine($"Итальянский язык");
            Console.WriteLine("==============================================");
            double italianEntropyBinary = EntropyCalculator.CalculateEntropy(italianTextBinary, BinaryAlphabet, "frequency_data_italian_binary.xlsx");
            Console.WriteLine($"Энтропия для итальянского бинарного текста: {italianEntropyBinary:F4}");
            Console.WriteLine("==============================================\n");

            Console.WriteLine($"\nМонгольский язык");
            Console.WriteLine("==============================================");
            double mongolianEntropyBinary = EntropyCalculator.CalculateEntropy(mongolianTextBinary, BinaryAlphabet, "frequency_data_mongolian_binary.xlsx");
            Console.WriteLine($"Энтропия для монгольского бинарного текста: {mongolianEntropyBinary:F4}");
            Console.WriteLine("==============================================\n");

            // задание в

            Console.WriteLine("Задание в\n");

            string italianFIO = "Vodchyts Anastasiya Vitalievna";
            string mongolianFIO = "Водчиц Анастасия Виталиевна";

            string italianFIOBinary = "01010110 01101111 01100100 01100011 01101000 01111001 01110100 01110011 00100000 01000001 01101110 01100001 01110011 01110100 01100001 01110011 01101001 01111001 01100001 00100000 01010110 01101001 01110100 01100001 01101100 01101001 01100101 01110110 01101110 01100001";
            string mongolianFIOBinary = "11010000 10010010 11010000 10111110 11010000 10110100 11010001 10000111 11010000 10111000 11010001 10000110 00100000 11010000 10010000 11010000 10111101 11010000 10110000 11010001 10000001 11010001 10000010 11010000 10110000 11010001 10000001 11010000 10111000 11010001 10001111 00100000 11010000 10010010 11010000 10111000 11010001 10000010 11010000 10110000 11010000 10111011 11010000 10111000 11010000 10110101 11010000 10110010 11010000 10111101 11010000 10110000";

            double italianInformationAmount = EntropyCalculator.CalculateInformationAmount(italianFIO, italianEntropy, ItalianAlphabet);
            double mongolianInformationAmount = EntropyCalculator.CalculateInformationAmount(mongolianFIO, mongolianEntropy, MongolianAlphabet);

            double italianInformationAmountBinary = EntropyCalculator.CalculateInformationAmount(italianFIOBinary, italianEntropyBinary, BinaryAlphabet);

            Console.WriteLine($"mongolianEntropyBinary: {mongolianEntropyBinary}");

            double mongolianInformationAmountBinary = EntropyCalculator.CalculateInformationAmount(mongolianFIOBinary, mongolianEntropyBinary, BinaryAlphabet);

            Console.WriteLine($"Количество информации в ФИО на итальянском: {italianInformationAmount:F4} бит");
            Console.WriteLine($"Количество информации в ФИО на монгольском: {mongolianInformationAmount:F4} бит");

            Console.WriteLine($"Количество информации в ФИО на итальянском (бинарный): {italianInformationAmountBinary:F4}  бит");
            Console.WriteLine($"Количество информации в ФИО на монгольском (бинарный): {mongolianInformationAmountBinary:F4}  бит");

            // задание г

            Console.WriteLine("\nЗадание в\n");

            double EffectiveEntropy = 1 - EntropyCalculator.EffectiveEntropy(0.1);

            Console.WriteLine($"Количество информации в ФИО на итальянском с ошибкой 0.1: {italianInformationAmountBinary * EffectiveEntropy:F4} бит");
            Console.WriteLine($"Количество информации в ФИО на монгольском с ошибкой 0.1: {mongolianInformationAmountBinary * EffectiveEntropy:F4} бит");

            EffectiveEntropy = 1 - EntropyCalculator.EffectiveEntropy(0.5);

            Console.WriteLine($"Количество информации в ФИО на итальянском с ошибкой 0.5: {italianInformationAmountBinary * EffectiveEntropy:F4} бит");
            Console.WriteLine($"Количество информации в ФИО на монгольском с ошибкой 0.5: {mongolianInformationAmountBinary * EffectiveEntropy:F4} бит");

            EffectiveEntropy = 1 - EntropyCalculator.EffectiveEntropy(1.0);

            Console.WriteLine($"EffectiveEntropy: {EffectiveEntropy}");

            Console.WriteLine($"Количество информации в ФИО на итальянском с ошибкой 1.0: {italianInformationAmountBinary * EffectiveEntropy:F4} бит");
            Console.WriteLine($"Количество информации в ФИО на монгольском с ошибкой 1.0: {mongolianInformationAmountBinary * EffectiveEntropy:F4} бит");

        }
    }
}