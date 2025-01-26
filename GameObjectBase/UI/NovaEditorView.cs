using GameObjectBase.UI.Utils;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase.UI
{
    public class NovaEditorView
    {
        public void GuiDraw()
        {
            uint mainDockSpaceId = ImGui.GetID("MainDockSpace");
            ImGuiViewportPtr viewport = ImGui.GetMainViewport();
            SharpDX.Vector2 windowSize = ImGui.GetWindowSize();
            ImGui.DockSpaceOverViewport(viewport);

            SplitHelper.SetSplitID("MainDockSpace", mainDockSpaceId);

            if (dockingParams.layoutReset)
            {
                Console.WriteLine("C# Nova Editor Gui Drawing Start!");
                ImGui.SetWindowPos(SharpDX.Vector2.Zero);
                ImGui.SetWindowSize(ImGui.GetIO().DisplaySize);

                var layout = NovaRenderer.CreateDefaultLayout(appState);
                dockingParams = layout[0];

                dockingParams.layoutReset = false;

                foreach (var dockingSplit in dockingParams.dockingSplits)
                {
                    //Note: Attaching windows to dockspaces, dock building & splitting are not implemented in ImGui.NET version
                    //      Until that, docking splits are useless
                }
            }


            foreach (var dockableWindow in dockingParams.dockableWindows)
            {
                if (dockableWindow.windowSize.X > 0.0f)
                    ImGui.SetNextWindowSize(dockableWindow.windowSize, dockableWindow.windowSizeCondition);
                if (dockableWindow.windowPosition.X > 0.0f)
                    ImGui.SetNextWindowPos(dockableWindow.windowPosition, dockableWindow.windowPositionCondition);

                bool notCollapsed = true;
                bool pOpen = false;
                if (dockableWindow.canBeClosed)
                {
                    notCollapsed = ImGui.Begin(dockableWindow.label, ref dockableWindow.isVisible, dockableWindow.imGuiWindowFlags);
                }
                else
                {
                    notCollapsed = ImGui.Begin(dockableWindow.label, ref pOpen, dockableWindow.imGuiWindowFlags);
                }
                if (notCollapsed && dockableWindow.GuiFunction != null)
                {
                    dockableWindow.GuiFunction();
                }

                ImGui.End();
            }
        }

        AppState appState = new AppState();
        DockingParams dockingParams = new DockingParams();
    }
}