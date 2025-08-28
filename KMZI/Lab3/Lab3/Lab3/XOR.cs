using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Lab3
{
    class XOR
    {
        public static byte[] XORBuffers(byte[] a, byte[] b)
        {
            int maxLength = Math.Max(a.Length, b.Length);
            byte[] result = new byte[maxLength];

            for (int i = 0; i < maxLength; i++)
            {
                byte byteA = (i < a.Length) ? a[i] : (byte)0;
                byte byteB = (i < b.Length) ? b[i] : (byte)0;
                result[i] = (byte)(byteA ^ byteB);
            }

            return result;
        }
        public static string ToBinary(byte[] data) => string.Join(" ", data.Select(b => Convert.ToString(b, 2).PadLeft(8, '0')));

    }
}
