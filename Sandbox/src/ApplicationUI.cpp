#include "ApplicationUI.h"
#include "DockingParams.h"

void GuiWindowViewport(AppState& appState)
{
	//ImGui::Text("Viewport here");
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::Image((ImTextureID)appState.outputTexture, ImVec2(windowSize.x, windowSize.y),
	             ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
}

void GuiWindowAssetBrowser(AppState& appState)
{
	ImGui::Text("AssetBrowser here");
}

void GuiWindowOutliner(AppState& appState)
{
	ImGui::Text("Outliner here");
}

void GuiWindowInspector(AppState& appState)
{
	ImGui::Text("Inspector here");
}

void GuiWindowStartPanel(AppState& appState)
{
	ImGui::Text("StartPanel here");
}

void GuiWindowLogger(AppState& appState)
{
	ImGui::Text("Logger here");
}

std::vector<DockingParams> CreateDefaultLayout(AppState& appState)
{
	DockingParams dockingParams;
	dockingParams.dockingSplits = CreateDefaultDockingSplits();
	dockingParams.dockableWindows = CreateDockableWindows(appState);

	return {dockingParams};
}

std::vector<DockingSplit> CreateDefaultDockingSplits()
{
	DockingSplit splitViewportToAssetBrowser;
	splitViewportToAssetBrowser.initialDock = "MainDockSpace";
	splitViewportToAssetBrowser.newDock = "AssetBrowserSpace";
	splitViewportToAssetBrowser.direction = ImGuiDir_Down;
	splitViewportToAssetBrowser.ratio = 0.5f;

	DockingSplit splitViewportToOutliner;
	splitViewportToOutliner.initialDock = "MainDockSpace";
	splitViewportToOutliner.newDock = "OutlinerSpace";
	splitViewportToOutliner.direction = ImGuiDir_Left;
	splitViewportToOutliner.ratio = 0.5f;

	DockingSplit splitViewportToInspector;
	splitViewportToInspector.initialDock = "MainDockSpace";
	splitViewportToInspector.newDock = "InspectorSpace";
	splitViewportToInspector.direction = ImGuiDir_Right;
	splitViewportToInspector.ratio = 0.75f;

	DockingSplit splitViewportToStartPanel;
	splitViewportToStartPanel.initialDock = "MainDockSpace";
	splitViewportToStartPanel.newDock = "StartPanelSpace";
	splitViewportToStartPanel.direction = ImGuiDir_Up;
	splitViewportToStartPanel.ratio = 0.25f;

	DockingSplit splitAssetBrowserToLogger;
	splitAssetBrowserToLogger.initialDock = "AssetBrowserSpace";
	splitAssetBrowserToLogger.newDock = "LoggerSpace";
	splitAssetBrowserToLogger.direction = ImGuiDir_Right;
	splitAssetBrowserToLogger.ratio = 0.3f;

	std::vector<DockingSplit> splits{
		splitViewportToAssetBrowser,
		splitViewportToOutliner,
		splitViewportToInspector,
		splitViewportToStartPanel,
		splitAssetBrowserToLogger
	};

	return splits;
}

std::vector<DockableWindow> CreateDockableWindows(AppState& appState)
{
	DockableWindow viewportWindow;
	viewportWindow.label = "Viewport";
	viewportWindow.dockSpaceName = "MainDockSpace";
	viewportWindow.GuiFunction = [&appState]() { GuiWindowViewport(appState); };

	DockableWindow startPanelWindow;
	startPanelWindow.label = "Start Panel";
	startPanelWindow.dockSpaceName = "StartPanelSpace";
	startPanelWindow.GuiFunction = [&] { GuiWindowStartPanel(appState); };

	DockableWindow outlinerWindow;
	outlinerWindow.label = "Outliner";
	outlinerWindow.dockSpaceName = "OutlinerSpace";
	outlinerWindow.GuiFunction = [&appState]() { GuiWindowOutliner(appState); };

	DockableWindow inspectorWindow;
	inspectorWindow.label = "Inspector";
	inspectorWindow.dockSpaceName = "InspectorSpace";
	inspectorWindow.imGuiWindowFlags = ImGuiWindowFlags_MenuBar;
	inspectorWindow.GuiFunction = [&appState]() { GuiWindowInspector(appState); };

	DockableWindow assetBrowserWindow;
	assetBrowserWindow.label = "Asset Browser";
	assetBrowserWindow.dockSpaceName = "AssetBrowserSpace";
	assetBrowserWindow.GuiFunction = [&appState]() { GuiWindowAssetBrowser(appState); };

	DockableWindow loggerWindow;
	loggerWindow.label = "Logger";
	loggerWindow.dockSpaceName = "LoggerSpace";
	loggerWindow.GuiFunction = [&appState]() { GuiWindowLogger(appState); };

	std::vector<DockableWindow> dockableWindows{
		startPanelWindow,
		viewportWindow,
		outlinerWindow,
		inspectorWindow,
		assetBrowserWindow,
		loggerWindow
	};

	return dockableWindows;
}