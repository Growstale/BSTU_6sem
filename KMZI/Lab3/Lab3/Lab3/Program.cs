using Lab3;
using System;
using System.Buffers.Text;
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

            string italianText = FileReader.ReadTextFromFile("italianText.txt");
            string base64Text = FileReader.ReadTextFromFile("encoded-20250302175431.txt");

            char[] ItalianAlphabet = "abcdefghijklmnopqrstuvwxyzàèéìíîòóùú".ToCharArray();
            char[] Base64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=".ToCharArray();

            Console.WriteLine("==================================================");
            double italianEntropy = EntropyCalculator.CalculateEntropy(italianText, ItalianAlphabet, "frequency_data_italian.xlsx");
            double italianEntropyHartly = EntropyCalculator.CalculateEntropyHartly(ItalianAlphabet);
            Console.WriteLine($"Энтропия для итальянского текста по Шеннону: {italianEntropy:F4}");
            Console.WriteLine($"Энтропия для итальянского текста по Хартли: {italianEntropyHartly:F4}");
            Console.WriteLine($"Избыточность алфавита: {EntropyCalculator.AlphabetRedundancy(italianEntropy, italianEntropyHartly):F4}%");
            Console.WriteLine("==================================================\n");

            Console.WriteLine("\n==================================================");
            double base64Entropy = EntropyCalculator.CalculateEntropy(base64Text, Base64Alphabet, "frequency_data_base64.xlsx");
            double base64EntropyHartly = EntropyCalculator.CalculateEntropyHartly(Base64Alphabet);
            Console.WriteLine($"Энтропия для документа формата base64 по Шеннону: {base64Entropy:F4}");
            Console.WriteLine($"Энтропия для документа формата base64 по Хартли: {base64EntropyHartly:F4}");
            Console.WriteLine($"Избыточность алфавита: {EntropyCalculator.AlphabetRedundancy(base64Entropy, base64EntropyHartly):F4}%");
            Console.WriteLine("==================================================\n");


            string sname = "Vodchyts";
            string fname = "Anastasiya";

            byte[] a_ascii = Encoding.ASCII.GetBytes(sname);
            byte[] b_ascii = Encoding.ASCII.GetBytes(fname);

            string a_Base64 = Convert.ToBase64String(a_ascii);
            string b_Base64 = Convert.ToBase64String(b_ascii);

            byte[] a_Base64_byte = Convert.FromBase64String(a_Base64);
            byte[] b_Base64_byte = Convert.FromBase64String(b_Base64);

            byte[] result_ascii = XOR.XORBuffers(a_ascii, b_ascii);
            byte[] result_Base64 = XOR.XORBuffers(a_Base64_byte, b_Base64_byte);

            byte[] result_ascii_double = XOR.XORBuffers(result_ascii, b_ascii);
            byte[] result_Base64_double = XOR.XORBuffers(result_Base64, b_Base64_byte);

            Console.WriteLine("ASCII");
            Console.WriteLine("==================================================");
            Console.WriteLine("a: \t\t\t" + string.Join(" ", a_ascii));
            Console.WriteLine("b: \t\t\t" + string.Join(" ", b_ascii));
            Console.WriteLine("a (2): \t\t\t" + XOR.ToBinary(a_ascii));
            Console.WriteLine("b (2): \t\t\t" + XOR.ToBinary(b_ascii));
            Console.WriteLine("a XOR b (2): \t\t" + XOR.ToBinary(result_ascii));
            Console.WriteLine("a XOR b: \t\t" + string.Join(" ", result_ascii));
            Console.WriteLine("a XOR b XOR b (2): \t" + XOR.ToBinary(result_ascii_double));
            Console.WriteLine("a XOR b XOR b: \t\t" + string.Join(" ", result_ascii_double));
            Console.WriteLine("==================================================\n");

            Console.WriteLine("Base64");
            Console.WriteLine("==================================================");
            Console.WriteLine("a: \t\t\t" + a_Base64);
            Console.WriteLine("b: \t\t\t" + b_Base64);
            Console.WriteLine("a (2): \t\t\t" + XOR.ToBinary(a_Base64_byte));
            Console.WriteLine("b (2): \t\t\t" + XOR.ToBinary(b_Base64_byte));
            Console.WriteLine("a XOR b (2): \t\t" + XOR.ToBinary(result_Base64));
            Console.WriteLine("a XOR b: \t\t" + Convert.ToBase64String(result_Base64));
            Console.WriteLine("a XOR b XOR b (2): \t" + XOR.ToBinary(result_Base64_double));
            Console.WriteLine("a XOR b XOR b: \t\t" + Convert.ToBase64String(result_Base64_double));
            Console.WriteLine("==================================================\n");

        }
    }
}