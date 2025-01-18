#include "DockingParams.h"

struct MyAppSettings
{	
	InputTextData motto = InputTextData(
		"Hello, Dear ImGui\n"
		"Usefull hint!\n",
		true,             // multiline
		ImVec2(14.f, 3.f) // initial size (in em)
	);
	int value = 10;
};

struct AppState
{
	float f = 0.0f;
	int counter = 0;

	float rocket_launch_time = 0.f;
	float rocket_progress = 0.0f;

	enum class RocketState
	{
		Init,
		Preparing,
		Launched
	};
	RocketState rocket_state = RocketState::Init;

	MyAppSettings myAppSettings;
	ImFont* TitleFont = nullptr;
	ImFont* ColorFont = nullptr;
	ImFont* EmojiFont = nullptr;
	ImFont* LargeIconFont = nullptr;
};

std::vector<DockingParams> CreateDefaultLayout(AppState& appState);
std::vector<DockingSplit> CreateDefaultDockingSplits();
std::vector<DockableWindow> CreateDockableWindows(AppState& appState);