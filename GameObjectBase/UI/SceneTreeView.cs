using GameObjectsBase;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ImGuiNET;
using System.Runtime.InteropServices;
using InteractableGroupsAi.Other;
using System.Numerics;
using GlmSharp;

namespace GameObjectBase.UI
{
    public class SceneTreeView
    {

        public GameObject SelectedObject { get; set; }

        public void Render()
        {
            SharpDX.Vector2 vec = new SharpDX.Vector2(200, 600);
            SharpDX.Vector2 pos = new SharpDX.Vector2(720, 200);

            ImGui.PushStyleVar(ImGuiStyleVar.WindowBorderSize, 0.0f);
            ImGui.SetNextWindowBgAlpha(0);
            ImGui.SetNextWindowSize(vec);
            ImGui.SetNextWindowPos(pos);
            ImGui.Begin("Scene Tree", ImGuiWindowFlags.NoTitleBar);
            ImGui.PopStyleVar();
            vec = new SharpDX.Vector2(100, 200);
            ImGui.SetCursorPos(vec);
            ImGui.Text("Test");

            vec = new SharpDX.Vector2(100, 300);
            ImGui.SetCursorPos(vec);
            ImGui.Button("Test");

            List<GameObject> list = Root.getAllGameObjects();
            foreach (var item in list)
            {
                if (ImGui.CollapsingHeader(item.Name))
                {
                    if (ImGui.IsItemHovered())
                    {
                        if (ImGui.IsMouseClicked(0))
                        {
                            SelectedObject = item;
                            InternalCalls.GameObject_SetMaterialID((ulong)SelectedObject.Id.Value, 0);
                        }
                    }
                }
            }
            ImGui.End();


        }


    }
}
