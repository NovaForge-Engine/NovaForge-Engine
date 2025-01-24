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

namespace GameObjectBase.UI
{
    public class SceneTreeView
    {

        public GameObject SelectedObject { get; set; }

        public void Render()
        {
            SharpDX.Vector2 vec = new SharpDX.Vector2(200, 600);
            SharpDX.Vector2 pos = new SharpDX.Vector2(720, 200);
            ImGui.SetNextWindowSize(vec);
            ImGui.SetNextWindowPos(pos);
            ImGui.Begin("Scene Tree");
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
                        }
                    }
                }
            }
            ImGui.End();

        }


    }
}
