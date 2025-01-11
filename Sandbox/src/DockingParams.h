#pragma once
#include <string>
#include <imgui.h>


using DockSpaceName = std::string;

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