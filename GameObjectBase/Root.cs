using GameObjectsBase;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ImGuiNET;
using System.Numerics;
using GameObjectBase.UI;

namespace GameObjectBase
{

   
    public static class Root
    {
        static int x;

        private static NovaEditorView _editor;
        private static ObjectsHolder _holder;

        public static void Initialize(IntPtr context, IntPtr alloc, IntPtr free)
        {
            Console.WriteLine("C# Root Constructor!");
            _holder = new ObjectsHolder();
            _editor = new NovaEditorView();

            ImGui.SetCurrentContext(context);
            ImGui.SetAllocatorFunctions(alloc, free);

            Console.WriteLine("C# Root Constructor!");
        }

       

        public static void Update()
        {

           // Console.WriteLine("C# Root Update!");
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
            _editor.GuiDraw();
        }



        public static List<GameObject> getAllGameObjects() => _holder.GetAllGameObjects();
    }

   
   
    

     
}
