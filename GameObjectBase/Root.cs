using GameObjectsBase;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ImGuiNET;
using System.Numerics;
using GameObjectBase.UI;
using GameObjectBase.GameComponents;

namespace GameObjectBase
{

   
    public static class Root
    {
        static int x;

        private static SceneTreeView _view;
        private static ObjectsHolder _holder;
        private static FieldInspector _fieldInspector;


        public static void Initialize(IntPtr context, IntPtr alloc, IntPtr free)
        {
            Console.WriteLine("C# Root Constructor!");
            _holder = new ObjectsHolder();
            _view = new SceneTreeView();
            _fieldInspector = new FieldInspector();

            InputManager.Init();

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
            //Console.WriteLine("C# Root Gui Start!");
           // Console.WriteLine();
            ImGui.Begin("Hello, World!");
            ImGui.Text("Hello imgui from c#.");
            ImGui.End();

            //ImGui.SliderInt("X in c#", ref x, 0, 100);
           //Console.WriteLine("this is x in c#  " + x);

            ImGui.Begin("Hello, World!");
            ImGui.Text("This is some useful text2.");



            if (ImGui.Button("button"))
            {
            int objid = 1;
                //0
                Console.WriteLine(objid);
            _holder.AddNewObject();
            InternalCalls.GameObject_AddGameObject();

            InternalCalls.GameObject_SetMeshID((ulong)objid, 7);
            InternalCalls.GameObject_SetMaterialID((ulong)objid, 2);

            objid++;
                Console.WriteLine(objid);
                //1
                _holder.AddNewObject();
                InternalCalls.GameObject_AddGameObject();

                InternalCalls.GameObject_SetMeshID((ulong)objid, 7);
                InternalCalls.GameObject_SetMaterialID((ulong)objid, 2);

                GameObject obj = _holder.FindObjectById(objid);
                obj.AddComponent(new TransformComponent());
                TransformComponent comp = obj.GetComponent<TransformComponent>();
                comp.SetPosition(new GlmSharp.vec3(0, 0, -25));

                objid++;
                Console.WriteLine(objid);
                //2
                _holder.AddNewObject();
            InternalCalls.GameObject_AddGameObject();

            InternalCalls.GameObject_SetMeshID((ulong)objid, 7);
            InternalCalls.GameObject_SetMaterialID((ulong)objid, 2);

            obj = _holder.FindObjectById(objid);
            obj.AddComponent(new TransformComponent());
            comp = obj.GetComponent<TransformComponent>();
            comp.SetPosition(new GlmSharp.vec3(0, 0, 25));

            objid++;
                Console.WriteLine(objid);
                //3

                _holder.AddNewObject();
            InternalCalls.GameObject_AddGameObject();

            InternalCalls.GameObject_SetMeshID((ulong)objid,(int)MaterialMap.ObjectName.GG_idle1);
            InternalCalls.GameObject_SetMaterialID((ulong)objid, (int)MaterialMap.MaterialName.GGIdle);

            obj = _holder.FindObjectById(objid);
            obj.AddComponent(new TransformComponent());
                obj.AddComponent(new PhysicsComponent());
                obj.SetName("player");
                comp = obj.GetComponent<TransformComponent>();
            comp.SetPosition(new GlmSharp.vec3(0, 0, -5));

                objid++;
                Console.WriteLine(objid);
                //4
                _holder.AddNewObject();
            InternalCalls.GameObject_AddGameObject();

            InternalCalls.GameObject_SetMeshID((ulong)objid, (int)MaterialMap.ObjectName.GG_idle1);
            InternalCalls.GameObject_SetMaterialID((ulong)objid,(int)MaterialMap.MaterialName.GGDamaged);

                obj = _holder.FindObjectById(objid);
                obj.AddComponent(new TransformComponent());
   
                comp = obj.GetComponent<TransformComponent>();
                comp.SetPosition(new GlmSharp.vec3(0, 0, 5));

                objid++;
                Console.WriteLine(objid);
                //5
                _holder.AddNewObject();
            InternalCalls.GameObject_AddGameObject();

            InternalCalls.GameObject_SetMeshID((ulong)objid, (int)MaterialMap.ObjectName.GG_idle1);
            InternalCalls.GameObject_SetMaterialID((ulong)objid, (int)MaterialMap.MaterialName.GGHealed);


                obj = _holder.FindObjectById(objid);
                obj.AddComponent(new TransformComponent());

                comp = obj.GetComponent<TransformComponent>();
                comp.SetPosition(new GlmSharp.vec3(-5, 0, 0));

                objid++;
                //6

                _holder.AddNewObject();
                InternalCalls.GameObject_AddGameObject();

                InternalCalls.GameObject_SetMeshID((ulong)objid, (int)MaterialMap.ObjectName.Monster_idle1);
                InternalCalls.GameObject_SetMaterialID((ulong)objid, (int)MaterialMap.MaterialName.Monster);


                obj = _holder.FindObjectById(objid);
                obj.AddComponent(new TransformComponent());
                comp = obj.GetComponent<TransformComponent>();
                comp.SetPosition(new GlmSharp.vec3(0, 0, 0));


                obj.AddComponent(new MovementController());

                GameObject obj3 = _holder.FindObjectById(4);
                TransformComponent comp3 = obj3.GetComponent<TransformComponent>();
                MovementController component = obj.GetComponent<MovementController>();
                component.target = comp3;


                objid++;

                //7

                _holder.AddNewObject();
                InternalCalls.GameObject_AddGameObject();

                InternalCalls.GameObject_SetMeshID((ulong)objid, (int)MaterialMap.ObjectName.Monster_idle2);
                InternalCalls.GameObject_SetMaterialID((ulong)objid, (int)MaterialMap.MaterialName.MonsterDamaged);


                obj = _holder.FindObjectById(objid);
                obj.AddComponent(new TransformComponent());
                comp = obj.GetComponent<TransformComponent>();
                comp.SetPosition(new GlmSharp.vec3(3, 0, 0));


                obj.AddComponent(new MovementController());

                GameObject obj2 = _holder.FindObjectById(4);
                TransformComponent comp2 = obj2.GetComponent<TransformComponent>();
                MovementController component2 = obj.GetComponent<MovementController>();
                component2.target = comp2;

                objid++;


                //8

                _holder.AddNewObject();
                InternalCalls.GameObject_AddGameObject();

                InternalCalls.GameObject_SetMeshID((ulong)objid, (int)MaterialMap.ObjectName.Cart);
                InternalCalls.GameObject_SetMaterialID((ulong)objid, (int)MaterialMap.MaterialName.Cart);


                obj = _holder.FindObjectById(objid);
                obj.AddComponent(new TransformComponent());
                comp = obj.GetComponent<TransformComponent>();
                comp.SetPosition(new GlmSharp.vec3(0, 0, 0));
                comp.SetScale(new GlmSharp.vec3(0.3f, 0.3f, 0.3f));



                objid++;

            }
            ImGui.Image((IntPtr)1000, new SharpDX.Vector2(100, 100));

            ImGui.Image((IntPtr)2000, new SharpDX.Vector2(100, 100));

            ImGui.End();

            _view.Render();
            _fieldInspector.Render(_view.SelectedObject);

           // Console.WriteLine("C# Root Gui End!");
        }



        public static List<GameObject> getAllGameObjects() => _holder.GetAllGameObjects();
    }

   
   
    

     
}
