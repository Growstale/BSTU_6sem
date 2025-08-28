using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Lab2
{
    class FileReader
    {
        public static string ReadTextFromFile(string filePath)
        {
            try
            {
                FileInfo fileInfo = new FileInfo(filePath);

                // Размер файла больше 30 КБ (10 * 1024 байт)
                if (fileInfo.Length < 30 * 1024)
                {
                    throw new Exception("Ошибка: файл слишком мал (меньше 10 КБ). Добавьте больше данных");
                }

                return File.ReadAllText(filePath);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Ошибка при чтении файла: {ex.Message}");
                return string.Empty;
            }

        }
    }
}