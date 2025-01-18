#include <imgui_internal.h>

#include "DockingParams.h"

namespace DockingDetails
{
   ImGuiWindowFlags WindowFlagsNothing()
   {
       ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
       windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar 
          | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus 
          | ImGuiWindowFlags_NoNavFocus;
       return windowFlags;
   }

   void DoSplit(const DockingSplit& dockingSplit)
   {
       IM_ASSERT(SplitIdsHelper::ContainsSplit(dockingSplit.initialDock) && "DoSplit: initialDock not found in gImGuiSplitIDs");
   
       ImGuiID initialDock_imguiId = SplitIdsHelper::GetSplitId(dockingSplit.initialDock);
       ImGuiID newDock_imguiId = ImGui::DockBuilderSplitNode(
               initialDock_imguiId,
               dockingSplit.direction,
               dockingSplit.ratio,
               nullptr,
               &initialDock_imguiId
       );
   
       SplitIdsHelper::SetSplitId(dockingSplit.initialDock, initialDock_imguiId);
       SplitIdsHelper::SetSplitId(dockingSplit.newDock, newDock_imguiId);
   
       // apply flags
       ImGuiDockNode* newDockNode = ImGui::DockBuilderGetNode(newDock_imguiId);
       newDockNode->SetLocalFlags(dockingSplit.nodeFlags);
   }

}