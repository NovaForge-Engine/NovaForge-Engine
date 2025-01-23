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
        static int x;
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
            //Console.WriteLine("C# Root Update!");
            _holder.Update();
        }

        public static void FixedUpdate()
        {
            _holder.FixedUpdate();
        }

        public static void ProcessInput(string name, int button){
            Console.WriteLine("C# Root Process Input! with " + name + " " + button);
        }


        public static void DrawGui()
        {
            //Console.WriteLine("C# Root Gui Start!");
            //Console.WriteLine();
            ImGui.Begin("Hello, World!");
            ImGui.Text("Hello imgui from c#.");
            ImGui.End();

            ImGui.SliderInt("X in c#", ref x, 0, 100);
            //Console.WriteLine("this is x in c#  " +  x);

            ImGui.Begin("Hello, World!");
            ImGui.Text("This is some useful text2.");


            if (ImGui.Button("button"))
            {
                _holder.AddNewObject();
            }

            ImGui.End();

            //Console.WriteLine("C# Root Gui End!");
        }
    }
}
