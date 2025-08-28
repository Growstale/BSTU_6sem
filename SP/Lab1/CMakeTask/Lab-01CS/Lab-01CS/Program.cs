using System;

class Program
{
    static void Main()
    {
        int x, k = 0;

        Console.Write("Enter a three-digit number: ");
        if (int.TryParse(Console.ReadLine(), out x))
        {
            if (x < 100 || x > 999)
            {
                Console.WriteLine("Error: a non-three-digit number was entered");
            }
            else
            {
                while (x > 0)
                {
                    k += x % 10;
                    x /= 10;
                }

                Console.WriteLine($"The sum of the digits of a number: {k}");
            }
        }
        else
        {
            Console.WriteLine("Error: invalid input.");
        }

        Console.WriteLine("Press Enter to exit...");
        Console.ReadLine();
    }
}