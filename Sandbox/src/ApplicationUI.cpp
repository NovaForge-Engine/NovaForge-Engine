#include "ApplicationUI.h"
#include "DockingParams.h"
#include <filesystem>
#include <algorithm>

const std::filesystem::path AppState::rootAssetPath = "Assets";

void DrawSceneItemRecursive(TmpTestSceneItem* sceneItem)
{
	if (ImGui::CollapsingHeader(sceneItem->itemName.c_str())) {
		for (auto childSceneItem : sceneItem->childs) {
			DrawSceneItemRecursive(childSceneItem);
		}
	}

	if (ImGui::BeginDragDropTarget()) {
		auto payload = ImGui::AcceptDragDropPayload("SceneItemPayload");
		if (payload && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			TmpTestSceneItem* draggedSceneItem = (TmpTestSceneItem*)payload->Data;
			if (draggedSceneItem->parent) {
				auto parentChilds = &draggedSceneItem->parent->childs;
				parentChilds->erase(std::remove(parentChilds->begin(), parentChilds->end(), draggedSceneItem), parentChilds->end());
			}
			sceneItem->childs.push_back(draggedSceneItem);
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("SceneItemPayload", sceneItem, sizeof(TmpTestSceneItem));
		ImGui::EndDragDropSource();
	}
}

void GuiWindowViewport(AppState& appState)
{
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::Image((ImTextureID)appState.outputTexture, ImVec2(windowSize.x, windowSize.y), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
}

void GuiWindowAssetBrowser(AppState& appState)
{
	if (appState.currentAssetDirectory != appState.rootAssetPath) {
		if (ImGui::Button("<-")) {
			appState.currentAssetDirectory = appState.currentAssetDirectory.parent_path();
			appState.dbgLogBuffer.push_back(TmpLogInfo("Current directory: " + appState.currentAssetDirectory.string(), TmpLogLevel::Warning));
		}
	}

	float cellSize = appState.assetThumbnailSize + appState.assetPadding;
	float panelWidth = ImGui::GetContentRegionAvail().x;

	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1) {
		columnCount = 1;
	}

	ImGui::Columns(columnCount, 0, false);

	for (auto& directoryEntry : std::filesystem::directory_iterator(appState.currentAssetDirectory)) { 
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(path, appState.rootAssetPath);
		std::string fileNameString = relativePath.filename().string();

		//Note: Must be ImageButton in the future
		if (ImGui::Button(fileNameString.c_str(), { appState.assetThumbnailSize, appState.assetThumbnailSize })) {
			appState.dbgLogBuffer.push_back(TmpLogInfo(fileNameString + " asset pressed", TmpLogLevel::Info));
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (directoryEntry.is_directory()) {
				appState.currentAssetDirectory /= path.filename();
				appState.dbgLogBuffer.push_back(TmpLogInfo("Current directory: " + appState.currentAssetDirectory.string(), TmpLogLevel::Warning));
			}
		}
		ImGui::Text(fileNameString.c_str());
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}

void GuiWindowOutliner(AppState& appState)
{
	const char* createAssetContextMenuID = "Create Asset";
	const char* removeAssetContextMenuID = "Remove Asset";

	for (auto sceneItem : appState.dbgSceneItems) {
		DrawSceneItemRecursive(&sceneItem);
	}

	if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		if (!ImGui::IsItemHovered()) {
			ImGui::OpenPopup(createAssetContextMenuID);
		}
		else {
			ImGui::OpenPopup(createAssetContextMenuID);
		}
	}

	if (ImGui::BeginPopup(createAssetContextMenuID)) {
		if (ImGui::MenuItem("Cube")) {
			appState.dbgSceneItems.push_back(TmpTestSceneItem("Cube"));
		}
		if (ImGui::MenuItem("Sphere")) {
			appState.dbgSceneItems.push_back(TmpTestSceneItem("Sphere"));
      }        
	   if (ImGui::MenuItem("Cylinder")) {
			appState.dbgSceneItems.push_back(TmpTestSceneItem("Cylinder"));
      }

     ImGui::EndPopup();
	}	

	if (ImGui::BeginPopup(removeAssetContextMenuID)) {
		if (ImGui::MenuItem("Remove")) {

		}
		ImGui::EndPopup();
	}
}

void GuiWindowInspector(AppState& appState)
{
	ImGui::Text("Inspector here");
}

void GuiWindowStartPanel(AppState& appState)
{
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::SetCursorPos(ImVec2((windowSize.x - appState.panelButtonSize) * 0.5f, windowSize.y  * 0.5f));

	ImVec4 buttonColor = !appState.isPlayMode ? ImVec4(0.0f, 0.67f, 0.0f, 1.0f) : ImVec4(0.67f, 0.0f, 0.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);

	const char* playButtonID = !appState.isPlayMode ? "|>" : "||";
	if (ImGui::Button(playButtonID)) {
		if (!appState.isPlayMode) {
			appState.dbgLogBuffer.push_back(TmpLogInfo("Entered play mode", TmpLogLevel::Debug));
		}
		else {
			appState.dbgLogBuffer.push_back(TmpLogInfo("Back to editor mode", TmpLogLevel::Error));
		}
		appState.isPlayMode = !appState.isPlayMode;
	}

	ImGui::PopStyleColor();
}

void GuiWindowLogger(AppState& appState)
{
	if (ImGui::Button("Clear logs")) {
		appState.dbgLogBuffer.clear();
	}

	for (auto log : appState.dbgLogBuffer) {
		ImGui::PushStyleColor(ImGuiCol_Text, TmpLogLevelToColor(log.logLevel));
		ImGui::Text(log.logBuffer.c_str());
		ImGui::PopStyleColor();
	}
}

ImVec4 TmpLogLevelToColor(TmpLogLevel logLevel) 
{
	switch (logLevel)
	{
	case Debug:
		return ImVec4(0.3f, 0.3f, 1.0f, 1.0f);
	case Info:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	case Warning:
		return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
	case Error:
		return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	default:
		return ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
	}
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
	splitViewportToAssetBrowser.ratio = 0.2f;

	DockingSplit splitViewportToOutliner;
	splitViewportToOutliner.initialDock = "MainDockSpace";
	splitViewportToOutliner.newDock = "OutlinerSpace";
	splitViewportToOutliner.direction = ImGuiDir_Left;
	splitViewportToOutliner.ratio = 0.15f;

	DockingSplit splitViewportToInspector;
	splitViewportToInspector.initialDock = "MainDockSpace";
	splitViewportToInspector.newDock = "InspectorSpace";
	splitViewportToInspector.direction = ImGuiDir_Right;
	splitViewportToInspector.ratio = 0.18f;

	DockingSplit splitViewportToStartPanel;
	splitViewportToStartPanel.initialDock = "MainDockSpace";
	splitViewportToStartPanel.newDock = "StartPanelSpace";
	splitViewportToStartPanel.direction = ImGuiDir_Up;
	splitViewportToStartPanel.ratio = 0.08f;

	DockingSplit splitAssetBrowserToLogger;
	splitAssetBrowserToLogger.initialDock = "AssetBrowserSpace";
	splitAssetBrowserToLogger.newDock = "LoggerSpace";
	splitAssetBrowserToLogger.direction = ImGuiDir_Right;
	splitAssetBrowserToLogger.ratio = 0.4f;

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
	viewportWindow.imGuiWindowFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	viewportWindow.GuiFunction = [&appState]() { GuiWindowViewport(appState); };

	DockableWindow startPanelWindow;
	startPanelWindow.label = "Start Panel";
	startPanelWindow.dockSpaceName = "StartPanelSpace";
	startPanelWindow.imGuiWindowFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize;
	startPanelWindow.GuiFunction = [&] { GuiWindowStartPanel(appState); };

	DockableWindow outlinerWindow;
	outlinerWindow.label = "Outliner";
	outlinerWindow.dockSpaceName = "OutlinerSpace";
	outlinerWindow.GuiFunction = [&appState]() { GuiWindowOutliner(appState); };

	DockableWindow inspectorWindow;
	inspectorWindow.label = "Inspector";
	inspectorWindow.dockSpaceName = "InspectorSpace";
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
		viewportWindow,
		startPanelWindow,
		outlinerWindow,
		inspectorWindow,
		assetBrowserWindow,
		loggerWindow
	};

	return dockableWindows;
}