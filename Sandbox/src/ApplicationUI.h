#include "DockingParams.h"
#include <NRIDescs.h>
#include <filesystem>

static unsigned int uniqueCounterId = 0;

struct TmpTestSceneItem
{
	TmpTestSceneItem(std::string itemName) : itemName(itemName + std::to_string(uniqueCounterId)) { uniqueCounterId++; }

	std::string itemName;
	
	TmpTestSceneItem* parent = nullptr;
	std::vector<TmpTestSceneItem*> childs;

private:
	TmpTestSceneItem();
};

enum TmpLogLevel
{
	Debug = 0,
	Info,
	Warning,
	Error
};

struct TmpLogInfo 
{
	TmpLogInfo(std::string log, TmpLogLevel logLevel) : logBuffer(logBuffer), logLevel(logLevel) {}

	std::string logBuffer;
	TmpLogLevel logLevel;
};

struct AppState
{
	nri::Descriptor* outputTexture = nullptr;

	std::vector<TmpTestSceneItem> dbgSceneItems = { TmpTestSceneItem("Cube"), TmpTestSceneItem("Sphere"), TmpTestSceneItem("Cylinder")};

	float assetPadding = 16.0f;
	float assetThumbnailSize = 120.0f;

	float panelButtonSize = 64.0f;
	bool isPlayMode = false;

	static const std::filesystem::path rootAssetPath;
	std::filesystem::path currentAssetDirectory = rootAssetPath;

	std::vector<TmpLogInfo> dbgLogBuffer;
};

ImVec4 TmpLogLevelToColor(TmpLogLevel logLevel);

std::vector<DockingParams> CreateDefaultLayout(AppState& appState);
std::vector<DockingSplit> CreateDefaultDockingSplits();
std::vector<DockableWindow> CreateDockableWindows(AppState& appState);