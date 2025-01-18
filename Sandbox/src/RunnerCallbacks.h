#pragma once
#include <functional>
#include <map>
#include <string>
#include <imgui.h>

using VoidFunction = std::function<void(void)>;
inline VoidFunction EmptyVoidFunction() { return {}; }

VoidFunction SequenceFunctions(const VoidFunction& f1, const VoidFunction& f2);

using AnyEventCallback = std::function<bool(void* backendEvent)>;
inline AnyEventCallback EmptyEventCallback() { return {}; }

enum class EdgeToolbarType
{
	Top,
	Bottom,
	Left,
	Right
};

struct EdgeToolbarOptions
{
	// height or width the top toolbar, in em units
	// (i.e. multiples of the default font size, to be Dpi aware)
	float sizeEm = 2.5f;

	// Padding inside the window, in em units
	ImVec2 WindowPaddingEm = ImVec2(0.3f, 0.3f);

	// Window background color, only used if WindowBg.w > 0
	ImVec4 WindowBg = ImVec4(0.f, 0.f, 0.f, 0.f);
};

struct EdgeToolbar
{
	VoidFunction ShowToolbar = EmptyVoidFunction();
	EdgeToolbarOptions options;
};

std::vector<EdgeToolbarType> AllEdgeToolbarTypes();
std::string EdgeToolbarTypeName(EdgeToolbarType e);

struct RunnerCallbacks
{
	// `ShowGui`: Fill it with a function that will add your widgets.
	// (ShowGui will be called at each frame, before rendering the Dockable windows, if any)
	VoidFunction ShowGui = EmptyVoidFunction();

	// `ShowMenus`: Fill it with a function that will add ImGui menus by
	// calling:
	//       ImGui::BeginMenu(...) / ImGui::MenuItem(...) / ImGui::EndMenu()
	//   Notes:
	//   * you do not need to call ImGui::BeginMenuBar and ImGui::EndMenuBar
	//   * Some default menus can be provided:
	//     see ImGuiWindowParams options:
	//         _showMenuBar, showMenu_App_QuitAbout, showMenu_View_
	VoidFunction ShowMenus = EmptyVoidFunction();

	// `ShowAppMenuItems`: A function that will render items that will be placed
	// in the App menu. They will be placed before the "Quit" MenuItem,
	// which is added automatically by HelloImGui.
	// This will be displayed only if ImGuiWindowParams.showMenu_App is true
	VoidFunction ShowAppMenuItems = EmptyVoidFunction();

	// `ShowStatus`: A function that will add items to the status bar.
	//  Use small items (ImGui::Text for example), since the height of the
	//  status is 30. Also, remember to call ImGui::SameLine() between items.
	VoidFunction ShowStatus = EmptyVoidFunction();

	// `EdgesToolbars`:
	// A dict that contains toolbars that can be placed on the edges of the App
	// window
	std::map<EdgeToolbarType, EdgeToolbar> edgesToolbars;

	// `AddEdgeToolbar`: Add a toolbar that can be placed on the edges of the
	// App window
	void AddEdgeToolbar(EdgeToolbarType edgeToolbarType, VoidFunction guiFunction, const EdgeToolbarOptions& options = EdgeToolbarOptions());

	// --------------- Startup sequence callbacks -------------------

	// `PostInit_AddPlatformBackendCallbacks`:
	//  You can here add a function that will be called once after OpenGL and
	//  ImGui are inited, but before the platform backend callbacks are
	//  initialized. If you, want to add your own glfw callbacks, you should use
	//  this function to do so (and then ImGui will call your callbacks followed
	//  by its own callbacks)
	VoidFunction PostInit_AddPlatformBackendCallbacks = EmptyVoidFunction();

	// `PostInit`: You can here add a function that will be called once after
	// everything
	//  is inited (ImGui, Platform and Renderer Backend)
	VoidFunction PostInit = EmptyVoidFunction();

	// `EnqueuePostInit`: Add a function that will be called once after OpenGL
	//  and ImGui are inited, but before the backend callback are initialized.
	//  (this will modify the `PostInit` callback by appending the new callback
	//  (using `SequenceFunctions`)
	void EnqueuePostInit(const VoidFunction& callback);
	
	// `RegisterTests`: A function that is called once ImGuiTestEngine is ready
	// to be filled with tests and automations definitions.
	VoidFunction RegisterTests = EmptyVoidFunction();
	// `registerTestsCalled`: will be set to true when RegisterTests was called
	// (you can set this to false if you want to RegisterTests to be called
	// again
	//  during the app execution)
	bool registerTestsCalled = false;

	// --------------- Exit sequence callbacks -------------------

	// `BeforeExit`: You can here add a function that will be called once before
	// exiting
	//  (when OpenGL and ImGui are still inited)
	VoidFunction BeforeExit = EmptyVoidFunction();

	// `EnqueueBeforeExit`: Add a function that will be called once before
	// exiting
	//  (when OpenGL and ImGui are still inited)
	// (this will modify the `BeforeExit` callback by appending the new callback
	// (using `SequenceFunctions`)
	void EnqueueBeforeExit(const VoidFunction& callback);

	// `BeforeExit_PostCleanup`: You can here add a function that will be called
	// once before exiting (after OpenGL and ImGui have been stopped)
	VoidFunction BeforeExit_PostCleanup = EmptyVoidFunction();

	// --------------- Callbacks in the render loop -------------------

	// `PreNewFrame`: You can here add a function that will be called at each
	// frame,
	//  and before the call to ImGui::NewFrame().
	//  It is a good place to add new dockable windows.
	VoidFunction PreNewFrame = EmptyVoidFunction();

	// `BeforeImGuiRender`: You can here add a function that will be called at
	// each frame,
	//  after the user Gui code, and just before the call to
	//  ImGui::Render() (which will also call ImGui::EndFrame()).
	VoidFunction BeforeImGuiRender = EmptyVoidFunction();

	// `AfterSwap`: You can here add a function that will be called at each
	// frame,
	//  after the Gui was rendered and swapped to the screen.
	VoidFunction AfterSwap = EmptyVoidFunction();

	// `CustomBackground`:
	//  By default, the background is cleared using
	//  ImGuiWindowParams.backgroundColor. If set, this function gives you full
	//  control over the background that is drawn behind the Gui. An example use
	//  case is if you have a 3D application like a mesh editor, or game, and
	//  just want the Gui to be drawn on top of that content.
	VoidFunction CustomBackground = EmptyVoidFunction();

	// `PostRenderDockableWindows`: Fill it with a function that will be called
	// after the dockable windows are rendered.
	VoidFunction PostRenderDockableWindows = EmptyVoidFunction();

	// `AnyBackendEventCallback`:
	//  Callbacks for events from a specific backend. _Only implemented for SDL.
	//  where the event will be of type 'SDL_Event *'_
	//  This callback should return true if the event was handled
	//  and shall not be processed further.
	//  Note: in the case of GLFW, you should use register them in `PostInit`
	AnyEventCallback AnyBackendEventCallback = EmptyEventCallback();

};