using GameObjectsBase;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ImGuiNET;
using System.Numerics;

namespace GameObjectBase
{
    public static class Root
    {
        private static ObjectsHolder _holder;

        public static void Initialize(IntPtr context, IntPtr alloc, IntPtr free)
        {
            Console.WriteLine("C# Root Constructor!");
            _holder = new ObjectsHolder();

            ImGui.SetCurrentContext(context);
            ImGui.SetAllocatorFunctions(alloc, free);
        }

        public static void Update()
        {
            Console.WriteLine("C# Root Update!");
            _holder.Update();
        }

        public static void FixedUpdate()
        {
            _holder.FixedUpdate();
        }

        public static void DrawGui()
        {
            Console.WriteLine("C# Root Gui Start!");
            Console.WriteLine();
            ImGui.Begin("Hello, World!");
            ImGui.Text("This is some useful text.");
            ImGui.End();
            Console.WriteLine("C# Root Gui End!");
        }
    }
}
