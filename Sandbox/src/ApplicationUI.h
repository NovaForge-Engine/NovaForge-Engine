#include "DockingParams.h"
#include <NRIDescs.h>
#include <filesystem>

enum TmpLogLevel
{
	Debug = 0,
	Info,
	Warning,
	Error
};

struct TmpLogInfo 
{
	std::string logBuffer;
	TmpLogLevel logLevel;

	TmpLogInfo(std::string log, TmpLogLevel logLevel) { this->logBuffer = log; this->logLevel = logLevel; }
};

struct AppState
{
	nri::Descriptor* outputTexture = nullptr;

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