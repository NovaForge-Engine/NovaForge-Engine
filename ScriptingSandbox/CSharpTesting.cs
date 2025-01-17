using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class CSharpTesting
{

    public float FloatVar { get; set; }

    public CSharpTesting()
    {
        Console.WriteLine("Main constructor!");
    }

    public void PrintMessage()
    {
        Console.WriteLine("Hello World from C#!");
    }

    public void PrintInt(int value)
    {
        Console.WriteLine($"C# says: {value}");
    }

    public void PrintInts(int value1, int value2)
    {
        Console.WriteLine($"C# says: {value1} and {value2}");
    }

    public void PrintCustomMessage(string message)
    {
        Console.WriteLine($"C# says: {message}");
    }

}
