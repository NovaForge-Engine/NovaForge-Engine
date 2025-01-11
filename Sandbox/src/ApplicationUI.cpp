#include "DockingParams.h"

std::vector<DockingSplit> CreateDefaultDockingSplits();
std::vector<DockableWindow> CreateDockableWindows(AppState& appState);

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

void GuiDefaultWindow(AppState& appState)
{
	//DemoFonts(appState);
	//ImGui::Separator();
	//DemoAssets(appState);
	//ImGui::Separator();
	//DemoLogs(appState);
	//ImGui::Separator();
	//DemoRocket(appState);
	//ImGui::Separator();
	//DemoUserSettings(appState);
	//ImGui::Separator();
	//DemoHideWindow(appState);
	//ImGui::Separator();
	//DemoShowAdditionalWindow(appState);
	//ImGui::Separator();
	//DemoThemes(appState);
	//ImGui::Separator();
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
	//    Define the default docking splits,
	//    i.e. the way the screen space is split in different target zones for
	//    the dockable windows
	//     We want to split "MainDockSpace" (which is provided automatically)
	//     into three zones, like this:
	//
	//    ___________________________________________
	//    |        |                                |
	//    | Command|                                |
	//    | Space  |    MainDockSpace               |
	//    |------- |                                |
	//    |        |--------------------------------|
	//    |        |       CommandSpace2            |
	//    -------------------------------------------
	//    |     MiscSpace                           |
	//    -------------------------------------------
	//

	// Then, add a space named "MiscSpace" whose height is 25% of the app
	// height. This will split the preexisting default dockspace "MainDockSpace"
	// in two parts.
	DockingSplit splitMainMisc;
	splitMainMisc.initialDock = "MainDockSpace";
	splitMainMisc.newDock = "MiscSpace";
	splitMainMisc.direction = ImGuiDir_Down;
	splitMainMisc.ratio = 0.25f;

	// Then, add a space to the left which occupies a column whose width is 25%
	// of the app width
	DockingSplit splitMainCommand;
	splitMainCommand.initialDock = "MainDockSpace";
	splitMainCommand.newDock = "CommandSpace";
	splitMainCommand.direction = ImGuiDir_Left;
	splitMainCommand.ratio = 0.25f;

	// Then, add CommandSpace2 below MainDockSpace
	DockingSplit splitMainCommand2;
	splitMainCommand2.initialDock = "MainDockSpace";
	splitMainCommand2.newDock = "CommandSpace2";
	splitMainCommand2.direction = ImGuiDir_Down;
	splitMainCommand2.ratio = 0.5f;

	std::vector<DockingSplit> splits{
		splitMainMisc, 
		splitMainCommand, 
		splitMainCommand2
	};

	return splits;
}

std::vector<DockableWindow> CreateDockableWindows(AppState& appState)
{
	// A window named "FeaturesDemo" will be placed in "CommandSpace". Its Gui
	// is provided by "GuiWindowDemoFeatures"
	DockableWindow featuresDemoWindow;
	featuresDemoWindow.label = "Features Demo";
	featuresDemoWindow.dockSpaceName = "CommandSpace";
	featuresDemoWindow.GuiFunction = [&] { GuiWindowDemoFeatures(appState); };

	// A layout customization window will be placed in "MainDockSpace". Its Gui
	// is provided by "GuiWindowLayoutCustomization"
	DockableWindow layoutCustomizationWindow;
	layoutCustomizationWindow.label = "Layout customization";
	layoutCustomizationWindow.dockSpaceName = "MainDockSpace";
	layoutCustomizationWindow.GuiFunction = [&appState]() { GuiWindowLayoutCustomization(appState); };

	// A Log window named "Logs" will be placed in "MiscSpace". It uses the
	// HelloImGui logger gui
	DockableWindow logsWindow;
	logsWindow.label = "Logs";
	logsWindow.dockSpaceName = "MiscSpace";
	logsWindow.GuiFunction = [] { HelloImGui::LogGui(); };

	// A Window named "Dear ImGui Demo" will be placed in "MainDockSpace"
	DockableWindow dearImGuiDemoWindow;
	dearImGuiDemoWindow.label = "Dear ImGui Demo";
	dearImGuiDemoWindow.dockSpaceName = "MainDockSpace";
	dearImGuiDemoWindow.imGuiWindowFlags = ImGuiWindowFlags_MenuBar;
	dearImGuiDemoWindow.GuiFunction = [] { ImGui::ShowDemoWindow(); };

	// alternativeThemeWindow
	DockableWindow alternativeThemeWindow;
	// Since this window applies a theme, We need to call "ImGui::Begin"
	// ourselves so that we can apply the theme before opening the window.
	alternativeThemeWindow.callBeginEnd = false;
	alternativeThemeWindow.label = "Alternative Theme";
	alternativeThemeWindow.dockSpaceName = "CommandSpace2";
	alternativeThemeWindow.GuiFunction = [&appState]() { GuiWindowAlternativeTheme(appState); };

	std::vector<DockableWindow> dockableWindows{
		featuresDemoWindow, 
		layoutCustomizationWindow, 
		logsWindow,
		dearImGuiDemoWindow, 
		alternativeThemeWindow
	};

	return dockableWindows;
}