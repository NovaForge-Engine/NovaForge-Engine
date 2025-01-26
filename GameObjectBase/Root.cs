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
        private static SceneTreeView _view;
        private static ObjectsHolder _holder;
        private static FieldInspector _fieldInspector;


        public static void Initialize(IntPtr context, IntPtr alloc, IntPtr free)
        {
            Console.WriteLine("C# Root Constructor!");
            _holder = new ObjectsHolder();
            _editor = new NovaEditorView();
            _view = new SceneTreeView();
            _fieldInspector = new FieldInspector();

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

            //ImGui.SliderInt("X in c#", ref x, 0, 100);
            //Console.WriteLine("this is x in c#  " + x);

            //ImGui.Begin("Hello, World!");
            //ImGui.Text("This is some useful text2.");



            //if (ImGui.Button("button"))
            //{
            //    _holder.AddNewObject();
            //}
            //ImGui.Image((IntPtr)1000, new SharpDX.Vector2(100, 100));
            //
            //ImGui.End();

            //_view.Render();
            //_fieldInspector.Render(_view.SelectedObject);

           // Console.WriteLine("C# Root Gui End!");
        }



        public static List<GameObject> getAllGameObjects() => _holder.GetAllGameObjects();
    }

   
   
    

     
}
