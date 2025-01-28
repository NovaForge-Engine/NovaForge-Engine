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
        private static string _scriptFilter = "";
        public void Render(GameObject selected)
        {
            if(selected == null) 
            {
                
                ImGui.Begin("Property window");
                ImGui.Text("No object selected");
                ImGui.End();
                return;
            }
            ImGui.Begin("Property window");

            var name = selected.NameRef;
            ImGui.InputText("Name", ref name, 30);
            selected.SetName(name);

            RenderTransform(selected);

            var assemblies = new List<Assembly>
            {
                Assembly.Load("GameObjectBase"),
            };

            var derivedTypes = new List<Type>();

            foreach (var assembly in assemblies)
            {
                try
                {
                    var types = assembly.GetTypes();
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

            if (ImGui.Button("Attach Script"))
                ImGui.OpenPopup("SelectScriptPopup");

            var comps = selected.GetComponents();

            if (ImGui.BeginPopup("SelectScriptPopup"))
            {
                ImGui.InputText("Search", ref _scriptFilter, 256);

                ImGui.Separator();
                foreach (var option in derivedTypes)
                {

                    if (string.IsNullOrEmpty(_scriptFilter) || option.Name.Contains(_scriptFilter))
                    {
                        if (ImGui.Selectable(option.Name))
                        {
                            //ConstructorInfo ctor = option.GetConstructor(new typeof(option)[]);
                            var component = Activator.CreateInstance(option) as Component;
                            selected.AddComponent(component);

                            _scriptFilter = "";
                            ImGui.CloseCurrentPopup();
                        }
                    }
                }

                ImGui.EndPopup();
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
            ImGui.SliderFloat4("Rotation", ref rot,0.0f,1.0f);
            ImGui.InputFloat3("Scale", ref scale);

            SharpDX.Quaternion q = new SharpDX.Quaternion(rot.X, rot.Y, rot.Z, rot.W);
            q.Normalize();
            component.SetPosition(MathTypes.toGLM(pos));
            component.SetRotation(MathTypes.toGLM(q));
            component.SetScale(MathTypes.toGLM(scale));

            InternalCalls.GameObject_SetWorldMatrix((ulong)selected.Id.Value, component.GetWorldMatrix());

        }

    }
}
