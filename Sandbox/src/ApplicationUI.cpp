#include "ApplicationUI.h"
#include "DockingParams.h"

void GuiWindowViewport(AppState& appState)
{
	ImGui::ShowDemoWindow();
}

void GuiWindowAssetBrowser(AppState& appState)
{
	ImGui::Separator();
}

void GuiWindowOutliner(AppState& appState)
{
	ImGui::Separator();
}

void GuiWindowInspector(AppState& appState)
{
	ImGui::Separator();
}

void GuiWindowStartPanel(AppState& appState)
{
	ImGui::Separator();
}

void GuiWindowLogger(AppState& appState)
{
	ImGui::Separator();
}

DockingParams CreateDefaultLayout(AppState& appState)
{
	DockingParams dockingParams;
	dockingParams.dockingSplits = CreateDefaultDockingSplits();
	dockingParams.dockableWindows = CreateDockableWindows(appState);
	return dockingParams;
}

std::vector<DockingSplit> CreateDefaultDockingSplits()
{
	DockingSplit splitViewportToAssetBrowser;
	splitViewportToAssetBrowser.initialDock = "MainDockSpace";
	splitViewportToAssetBrowser.newDock = "AssetBrowserSpace";
	splitViewportToAssetBrowser.direction = ImGuiDir_Down;
	splitViewportToAssetBrowser.ratio = 0.2f;

	DockingSplit splitViewportToOutliner;
	splitViewportToOutliner.initialDock = "MainDockSpace";
	splitViewportToOutliner.newDock = "OutlinerSpace";
	splitViewportToOutliner.direction = ImGuiDir_Left;
	splitViewportToOutliner.ratio = 0.25f;

	DockingSplit splitViewportToInspector;
	splitViewportToInspector.initialDock = "MainDockSpace";
	splitViewportToInspector.newDock = "InspectorSpace";
	splitViewportToInspector.direction = ImGuiDir_Right;
	splitViewportToInspector.ratio = 0.25f;

	DockingSplit splitViewportToStartPanel;
	splitViewportToStartPanel.initialDock = "MainDockSpace";
	splitViewportToStartPanel.newDock = "StartPanelSpace";
	splitViewportToStartPanel.direction = ImGuiDir_Up;
	splitViewportToStartPanel.ratio = 0.1f;

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
	DockableWindow startPanelWindow;
	startPanelWindow.label = "Start Panel";
	startPanelWindow.dockSpaceName = "StartPanelSpace";
	startPanelWindow.GuiFunction = [&] { GuiWindowStartPanel(appState); };

	DockableWindow viewportWindow;
	viewportWindow.label = "Viewport";
	viewportWindow.dockSpaceName = "MainDockSpace";
	viewportWindow.GuiFunction = [&appState]() { GuiWindowViewport(appState); };

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