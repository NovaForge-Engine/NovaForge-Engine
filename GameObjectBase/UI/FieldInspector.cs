using GameObjectsBase;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ImGuiNET;
using System.Diagnostics;
using GameObjectBase.GameComponents;
using GlmSharp;
using System.Reflection;
using System.IO;

namespace GameObjectBase.UI
{
    internal class FieldInspector
    {

        public void Render(GameObject selected)
        {
            if(selected == null) {
                ImGui.Begin("Property window");
                ImGui.Text("No object selected");
                ImGui.End();
                return;
            }
            ImGui.Begin("Property window");
            var name = selected.NameRef;
            ImGui.InputText("Name", ref name, 30);

            RenderTransform(selected);

            if(ImGui.Button("Add transform"))
            {
                TransformComponent trans = new TransformComponent();
                selected.AddComponent(trans);
            }

            var assemblies = new Assembly[]
           {
                Assembly.Load("GameObjectBase"),
                Assembly.LoadFrom("Resources\\Scripts\\GameObjectBase.dll"),
           };

            Console.WriteLine(System.AppDomain.CurrentDomain.BaseDirectory);
            Console.WriteLine(System.Environment.CurrentDirectory);
            Console.WriteLine(System.IO.Directory.GetCurrentDirectory());
            Console.WriteLine(Environment.CurrentDirectory);
            var derivedTypes = new List<Type>();

            foreach (var assembly in assemblies)
            {
                try
                {
                    var types = assembly.GetTypes();
                    Console.WriteLine(types);
                    derivedTypes.AddRange(types.Where(t =>
                        t.IsClass &&
                        !t.IsAbstract &&
                        t.IsSubclassOf(typeof(Component))));
                }
                catch (ReflectionTypeLoadException ex)
                {
                    foreach (var loaderException in ex.LoaderExceptions)
                    {
                        Console.WriteLine(loaderException);
                    }
                }
            }




            ImGui.End();
        }


        public void RenderTransform(GameObject selected)
        {
            TransformComponent component = selected.GetComponent<TransformComponent>();
            if(component == null) { return; }
            SharpDX.Vector3 pos= new SharpDX.Vector3(component.Position.x, component.Position.y, component.Position.z);
            SharpDX.Vector3 scale = new SharpDX.Vector3(component.Scale.x, component.Scale.y, component.Scale.z);
            SharpDX.Vector4 rot = new SharpDX.Vector4(component.Rotation.x, component.Rotation.y, component.Rotation.z, component.Rotation.w);

            ImGui.Separator();
            ImGui.InputFloat3("Position",ref pos);
            ImGui.InputFloat4("Rotation", ref rot);
            ImGui.InputFloat3("Scale", ref scale);

        }

    }
}
