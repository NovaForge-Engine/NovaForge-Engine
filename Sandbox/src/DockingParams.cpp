#include <string>
#include "DockingParams.h"

namespace SplitIdsHelper
{
	std::map<DockSpaceName, ImGuiID> gImGuiSplitIDs;

	bool ContainsSplit(const DockSpaceName& dockSpaceName)
	{
		return gImGuiSplitIDs.find(dockSpaceName) != gImGuiSplitIDs.end();
	}

	ImGuiID GetSplitId(const DockSpaceName& dockSpaceName)
	{
		IM_ASSERT(ContainsSplit(dockSpaceName) &&
		          "GetSplitId: dockSpaceName not found in gImGuiSplitIDs");
		return gImGuiSplitIDs.at(dockSpaceName);
	}

	void SetSplitId(const DockSpaceName& dockSpaceName, ImGuiID imguiId)
	{
		gImGuiSplitIDs[dockSpaceName] = imguiId;
	}
}

DockableWindow* DockingParams::DockableWindowOfName(const std::string& name)
{
	for (auto& dockableWindow : dockableWindows) {
		if (dockableWindow.label == name) {
			return &dockableWindow;
		}
	}
	return nullptr;
}

bool DockingParams::FocusDockableWindow(const std::string& windowName)
{
	DockableWindow* win = DockableWindowOfName(windowName);
	if (win != nullptr) {
		win->focusWindowAtNextFrame = true;
		return true;
	}
	else {
		return false;
	}
}

std::optional<ImGuiID> DockingParams::DockSpaceIdFromName(const std::string& dockSpaceName)
{
	if (SplitIdsHelper::ContainsSplit(dockSpaceName))
		return SplitIdsHelper::GetSplitId(dockSpaceName);
	else
		return std::nullopt;
}