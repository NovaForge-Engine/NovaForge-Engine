#pragma once
#include <string>
#include "imgui.h"
#include "RunnerCallbacks.h"
#include <optional>

using DockSpaceName = std::string;

namespace SplitIdsHelper
{
	bool ContainsSplit(const DockSpaceName& dockSpaceName);
	ImGuiID GetSplitId(const DockSpaceName& dockSpaceName);
	void SetSplitId(const DockSpaceName& dockSpaceName, ImGuiID imguiId);
}

struct DockingSplit
{
	//  id of the space that should be split.
	//  At the start, there is only one Dock Space named "MainDockSpace".
	//  You should start by partitioning this space, in order to create a new
	//  dock space.
	DockSpaceName initialDock;

	//  id of the new dock space that will be created.
	DockSpaceName newDock;

	//  (enum with ImGuiDir_Down, ImGuiDir_Down, ImGuiDir_Left, ImGuiDir_Right)*
	//  Direction where this dock space should be created.
	ImGuiDir direction;

	//  Ratio of the initialDock size that should be used by the new dock space.
	float ratio = 0.25f;

	//  Flags to apply to the new dock space
	//  (enable/disable resizing, splitting, tab bar, etc.)
	ImGuiDockNodeFlags nodeFlags = ImGuiDockNodeFlags_None;

	// Constructor
	DockingSplit(const DockSpaceName& initialDock_ = "", const DockSpaceName& newDock_ = "", ImGuiDir direction_ = ImGuiDir_Down, float ratio_ = 0.25f, ImGuiDockNodeFlags nodeFlags_ = ImGuiDockNodeFlags_None)
		: initialDock(initialDock_), newDock(newDock_), direction(direction_), ratio(ratio_), nodeFlags(nodeFlags_) {}
};

struct DockableWindow
{
	// --------------- Main params -------------------

	// `label`: _string_. Title of the window. It should be unique! Use "##" to
	// add a unique suffix if needed.
	std::string label;

	// `dockSpaceName`: _DockSpaceName (aka string)_.
	//  Id of the dock space where this window should initially be placed
	DockSpaceName dockSpaceName;

	// `GuiFunction`: _VoidFunction_.
	// Any function that will render this window's Gui
	VoidFunction GuiFunction = EmptyVoidFunction();

	// --------------- Options --------------------------

	// `isVisible`: _bool, default=true_.
	//  Flag that indicates whether this window is visible or not.
	bool isVisible = true;

	// `rememberIsVisible`: _bool, default=true_.
	//  Flag that indicates whether the window visibility should be saved in
	//  settings.
	bool rememberIsVisible = true;

	// `canBeClosed`: _bool, default=true_.
	//  Flag that indicates whether the user can close this window.
	bool canBeClosed = true;

	// `callBeginEnd`: _bool, default=true_.
	//  Flag that indicates whether ImGui::Begin and ImGui::End
	//  calls should be added automatically (with the given "label").
	//  Set to false if you want to call ImGui::Begin/End yourself
	bool callBeginEnd = true;

	// `includeInViewMenu`: _bool, default=true_.
	//  Flag that indicates whether this window should be mentioned in the view
	//  menu.
	bool includeInViewMenu = true;

	// `imGuiWindowFlags`: _ImGuiWindowFlags, default=0_.
	//  Window flags, see enum ImGuiWindowFlags_
	ImGuiWindowFlags imGuiWindowFlags = 0;

	// --------------- Focus window -----------------------------

	// `focusWindowAtNextFrame`: _bool, default = false_.
	//  If set to true this window will be focused at the next frame.
	bool focusWindowAtNextFrame = false;

	// --------------- Size & Position --------------------------
	//              (only if not docked)

	// `windowSize`: _ImVec2, default=(0.f, 0.f) (i.e let the app decide)_.
	//  Window size (unused if docked)
	ImVec2 windowSize = ImVec2(0.f, 0.f);

	// `windowSizeCondition`: _ImGuiCond, default=ImGuiCond_FirstUseEver_.
	//  When to apply the window size.
	ImGuiCond windowSizeCondition = ImGuiCond_FirstUseEver;

	// `windowPos`: _ImVec2, default=(0.f, 0.f) (i.e let the app decide)_.
	//  Window position (unused if docked)
	ImVec2 windowPosition = ImVec2(0.f, 0.f);

	// `windowPosCondition`: _ImGuiCond, default=ImGuiCond_FirstUseEver_.
	//  When to apply the window position.
	ImGuiCond windowPositionCondition = ImGuiCond_FirstUseEver;

	// Constructor
	DockableWindow(const std::string& label_ = "", const DockSpaceName& dockSpaceName_ = "", const VoidFunction guiFunction_ = EmptyVoidFunction(), bool isVisible_ = true, bool canBeClosed_ = true)
		: label(label_), dockSpaceName(dockSpaceName_), GuiFunction(guiFunction_), isVisible(isVisible_), canBeClosed(canBeClosed_) {}
};

enum class DockingLayoutCondition
{
	FirstUseEver,
	ApplicationStart,
	Never
};

struct DockingParams
{
	// --------------- Main params -----------------------------

	// `dockingSplits`: _vector[DockingSplit]_.
	//  Defines the way docking splits should be applied on the screen
	//  in order to create new Dock Spaces
	std::vector<DockingSplit> dockingSplits;

	// `dockableWindows`: _vector[DockableWindow]_.
	//  List of the dockable windows, together with their Gui code
	std::vector<DockableWindow> dockableWindows;

	// `layoutName`: _string, default="default"_.
	//  Displayed name of the layout.
	//  Only used in advanced cases, when several layouts are available.
	std::string layoutName = "Default";

	// --------------- Options -----------------------------

	// `mainDockSpaceNodeFlags`: _ImGuiDockNodeFlags (enum),
	//      default=ImGuiDockNodeFlags_PassthruCentralNode_
	//  Flags to apply to the main dock space
	//  (enable/disable resizing, splitting, tab bar, etc.).
	//  Most flags are inherited by children dock spaces.
	//  You can also set flags for specific dock spaces via
	//  `DockingSplit.nodeFlags`
	ImGuiDockNodeFlags mainDockSpaceNodeFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	// --------------- Layout handling -----------------------------

	// `layoutCondition`: _enum DockingLayoutCondition, default=FirstUseEver_.
	//  When to apply the docking layout. Choose between
	//      FirstUseEver (apply once, then keep user preference),
	//      ApplicationStart (always reapply at application start)
	//      Never
	DockingLayoutCondition layoutCondition = DockingLayoutCondition::FirstUseEver;

	// `layoutReset`: _bool, default=false_.
	//  Reset layout on next frame, i.e. drop the layout customizations which
	//  were applied manually by the user. layoutReset will be reset to false
	//  after this.
	bool layoutReset = false;

	// --------------- Helper Methods -----------------------------

	// `DockableWindow * dockableWindowOfName(const std::string & name)`:
	// returns a pointer to a dockable window
	DockableWindow* DockableWindowOfName(const std::string& name);

	// `bool focusDockableWindow(const std::string& name)`:
	// will focus a dockable window (and make its tab visible if needed)
	bool FocusDockableWindow(const std::string& windowName);

	// `optional<ImGuiID> dockSpaceIdFromName(const std::string&
	// dockSpaceName)`: returns the ImGuiID corresponding to the dockspace with
	// this name
	std::optional<ImGuiID> DockSpaceIdFromName(const std::string& dockSpaceName);
};

// (@Tenzy21) TODO: Move somewhere else
struct InputTextData
{
	// The text edited in the input field
	std::string text;

	// An optional hint displayed when the input field is empty
	// (only works for single-line text input)
	std::string hint;

	// If true, the input field is multi-line
	bool isMultiline = false;

	// If true, the input field is resizable
	bool isResizable = true;

	// The size of the input field in em units
	ImVec2 sizeEm = ImVec2(0, 0);

	InputTextData(const std::string& text = "", bool isMultiline = false, ImVec2 sizeEm = ImVec2(0, 0))
		: text(text), isMultiline(isMultiline), sizeEm(sizeEm) {}
};
