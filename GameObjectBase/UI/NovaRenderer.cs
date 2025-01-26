using GameObjectBase.UI.Utils;
using GameObjectsBase;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace GameObjectBase.UI
{
    public static class NovaRenderer
    {
        public static List<DockingParams> CreateDefaultLayout(AppState appState)
        {
            DockingParams dockingParams = new DockingParams();
            dockingParams.dockingSplits = CreateDefaultDockingSplits();
            dockingParams.dockableWindows = CreateDockableWindows(appState);
            return new List<DockingParams>{ dockingParams };
        }

        static List<DockingSplit> CreateDefaultDockingSplits()
        {
            DockingSplit splitViewportToAssetBrowser = new DockingSplit();
            splitViewportToAssetBrowser.initialDock = "MainDockSpace";
            splitViewportToAssetBrowser.newDock = "AssetBrowserSpace";
            splitViewportToAssetBrowser.direction = ImGuiDir.Down;
            splitViewportToAssetBrowser.ratio = 0.2f;

            DockingSplit splitViewportToOutliner = new DockingSplit();
            splitViewportToOutliner.initialDock = "MainDockSpace";
            splitViewportToOutliner.newDock = "OutlinerSpace";
            splitViewportToOutliner.direction = ImGuiDir.Left;
            splitViewportToOutliner.ratio = 0.15f;

            DockingSplit splitViewportToInspector = new DockingSplit();
            splitViewportToInspector.initialDock = "MainDockSpace";
            splitViewportToInspector.newDock = "InspectorSpace";
            splitViewportToInspector.direction = ImGuiDir.Right;
            splitViewportToInspector.ratio = 0.18f;

            DockingSplit splitViewportToStartPanel = new DockingSplit();
            splitViewportToStartPanel.initialDock = "MainDockSpace";
            splitViewportToStartPanel.newDock = "StartPanelSpace";
            splitViewportToStartPanel.direction = ImGuiDir.Up;
            splitViewportToStartPanel.ratio = 0.08f;

            DockingSplit splitAssetBrowserToLogger = new DockingSplit();
            splitAssetBrowserToLogger.initialDock = "AssetBrowserSpace";
            splitAssetBrowserToLogger.newDock = "LoggerSpace";
            splitAssetBrowserToLogger.direction = ImGuiDir.Right;
            splitAssetBrowserToLogger.ratio = 0.4f;

            return new List<DockingSplit>
            {
                splitViewportToAssetBrowser,
                splitViewportToOutliner,
                splitViewportToInspector,
                splitViewportToStartPanel,
                splitAssetBrowserToLogger
            };
        }

        static List<DockableWindow> CreateDockableWindows(AppState appState)
        {
            DockableWindow viewportWindow = new DockableWindow();
            viewportWindow.label = "Viewport";
            viewportWindow.dockSpaceName = "MainDockSpace";
            viewportWindow.imGuiWindowFlags |= ImGuiWindowFlags.NoScrollbar | ImGuiWindowFlags.NoScrollWithMouse;
            viewportWindow.GuiFunction = () => GuiWindowViewport(appState);

            DockableWindow startPanelWindow = new DockableWindow();
            startPanelWindow.label = "Start Panel";
            startPanelWindow.dockSpaceName = "StartPanelSpace";
            startPanelWindow.imGuiWindowFlags |= ImGuiWindowFlags.NoScrollbar | ImGuiWindowFlags.NoScrollWithMouse;
            startPanelWindow.GuiFunction = () => GuiWindowStartPanel(appState);

            DockableWindow outlinerWindow = new DockableWindow();
            outlinerWindow.label = "Outliner";
            outlinerWindow.dockSpaceName = "OutlinerSpace";
            outlinerWindow.GuiFunction = () => GuiWindowOutliner(appState);

            DockableWindow inspectorWindow = new DockableWindow();
            inspectorWindow.label = "Inspector";
            inspectorWindow.dockSpaceName = "InspectorSpace";
            inspectorWindow.GuiFunction = () => GuiWindowInspector(appState);

            DockableWindow assetBrowserWindow = new DockableWindow();
            assetBrowserWindow.label = "Asset Browser";
            assetBrowserWindow.dockSpaceName = "AssetBrowserSpace";
            assetBrowserWindow.GuiFunction = () => GuiWindowAssetBrowser(appState);

            DockableWindow loggerWindow = new DockableWindow();
            loggerWindow.label = "Logger";
            loggerWindow.dockSpaceName = "LoggerSpace";
            loggerWindow.GuiFunction = () => GuiWindowLogger(appState);

            return new List<DockableWindow>
            {
                viewportWindow,
	        	startPanelWindow,
	        	outlinerWindow,
	        	inspectorWindow,
	        	assetBrowserWindow,
	        	loggerWindow
            };
        }

        static void DrawGameObjectRecursive(GameObject gameObject)
        {
            if (ImGui.CollapsingHeader(gameObject.Name))
            { 
                var children = gameObject.GetChildren();
                if (children == null || children.Count() == 0)
                    return;

                foreach (var childGameObject in children)
                {
                    DrawGameObjectRecursive(childGameObject);
                }
            }
        }

        static void GuiWindowViewport(AppState appState)
        {
            SharpDX.Vector2 windowSize = ImGui.GetWindowSize();
            ImGui.Image(appState.outputTexture, windowSize, appState.textureUV0, appState.textureUV1, appState.textureTintCol, appState.textureBorderCol);
        }

        static void GuiWindowStartPanel(AppState appState)
        {
            SharpDX.Vector2 windowSize = ImGui.GetWindowSize();
            ImGui.SetCursorPos(new SharpDX.Vector2((windowSize.X - appState.panelButtonSize) * 0.5f, windowSize.Y * 0.5f));

            SharpDX.Vector4 buttonColor = !appState.isPlayMode ? appState.playButtonCol : appState.stopButtonCol;
            ImGui.PushStyleColor(ImGuiCol.Button, buttonColor);

            string playButtonID = !appState.isPlayMode ? "|>" : "||";
            if (ImGui.Button(playButtonID))
            {
                appState.isPlayMode = !appState.isPlayMode;
            }

            ImGui.PopStyleColor();
        }

        static void GuiWindowOutliner(AppState appState)
        {
            foreach (var gameObject in appState.gameObjects)
            {
                DrawGameObjectRecursive(gameObject);
            }
        }

        static void GuiWindowInspector(AppState appState)
        {

        }

        static void GuiWindowAssetBrowser(AppState appState)
        {

        }

        static void GuiWindowLogger(AppState appState)
        {

        }
    }
}
