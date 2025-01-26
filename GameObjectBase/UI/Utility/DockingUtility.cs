using ImGuiNET;
using System.Collections.Generic;
using GameObjectBase.UI;
using DockSpaceName = System.String;
using System.Reflection.Emit;
using System;
using GameObjectBase.UI.Utils;
using VoidFunction = System.Action;

namespace GameObjectBase.UI
{
    public static class SplitHelper
    {
        private static Dictionary<DockSpaceName, uint> gImGuiSplitIDs = new Dictionary<DockSpaceName, uint>();

        public static bool ContainsSplit(DockSpaceName dockspaceName)
        {
            return gImGuiSplitIDs.ContainsKey(dockspaceName);
        }

        public static uint GetSplitID(DockSpaceName dockspaceName)
        {
            return gImGuiSplitIDs[dockspaceName];
        }

        public static void SetSplitID(DockSpaceName dockspaceName, uint imguiId)
        {
            gImGuiSplitIDs[dockspaceName] = imguiId;
        }

        public static void DoSplit(DockingSplit dockingSplit)
        {
            uint initialDock_imguiId = GetSplitID(dockingSplit.initialDock);
        }
    }

    public class DockingSplit
    {
        //  id of the space that should be split.
        //  At the start, there is only one Dock Space named "MainDockSpace".
        //  You should start by partitioning this space, in order to create a new
        //  dock space.
        public DockSpaceName initialDock;

        //  id of the new dock space that will be created.
        public DockSpaceName newDock;

        //  (enum with ImGuiDir_Down, ImGuiDir_Down, ImGuiDir_Left, ImGuiDir_Right)*
        //  Direction where this dock space should be created.
        public ImGuiDir direction;

        //  Ratio of the initialDock size that should be used by the new dock space.
        public float ratio = 0.25f;

        //  Flags to apply to the new dock space
        //  (enable/disable resizing, splitting, tab bar, etc.)
        ImGuiDockNodeFlags nodeFlags = ImGuiDockNodeFlags.None;
    }

    public class DockableWindow
    {
        // --------------- Main params -------------------

        // `label`: _string_. Title of the window. It should be unique! Use "##" to
        // add a unique suffix if needed.
        public string label;

        // `dockSpaceName`: _DockSpaceName (aka string)_.
        //  Id of the dock space where this window should initially be placed
        public DockSpaceName dockSpaceName;

        // `GuiFunction`: _VoidFunction_.
        // Any function that will render this window's Gui
        public VoidFunction GuiFunction = ImGuiUtils.EmptyVoidFunction();

        // --------------- Options --------------------------

        // `isVisible`: _bool, default=true_.
        //  Flag that indicates whether this window is visible or not.
        public bool isVisible = true;

        // `rememberIsVisible`: _bool, default=true_.
        //  Flag that indicates whether the window visibility should be saved in
        //  settings.
        public bool rememberIsVisible = true;

        // `canBeClosed`: _bool, default=true_.
        //  Flag that indicates whether the user can close this window.
        public bool canBeClosed = true;

        // `callBeginEnd`: _bool, default=true_.
        //  Flag that indicates whether ImGui::Begin and ImGui::End
        //  calls should be added automatically (with the given "label").
        //  Set to false if you want to call ImGui::Begin/End yourself
        public bool callBeginEnd = true;

        // `includeInViewMenu`: _bool, default=true_.
        //  Flag that indicates whether this window should be mentioned in the view
        //  menu.
        public bool includeInViewMenu = true;

        // `imGuiWindowFlags`: _ImGuiWindowFlags, default=0_.
        //  Window flags, see enum ImGuiWindowFlags_
        public ImGuiWindowFlags imGuiWindowFlags = 0;

        // --------------- Focus window -----------------------------

        // `focusWindowAtNextFrame`: _bool, default = false_.
        //  If set to true this window will be focused at the next frame.
        public bool focusWindowAtNextFrame = false;

        // --------------- Size & Position --------------------------
        //              (only if not docked)

        // `windowSize`: _ImVec2, default=(0.f, 0.f) (i.e let the app decide)_.
        //  Window size (unused if docked)
        public SharpDX.Vector2 windowSize = new SharpDX.Vector2(0.0f);

        // `windowSizeCondition`: _ImGuiCond, default=ImGuiCond_FirstUseEver_.
        //  When to apply the window size.
        public ImGuiCond windowSizeCondition = ImGuiCond.FirstUseEver;

        // `windowPos`: _ImVec2, default=(0.f, 0.f) (i.e let the app decide)_.
        //  Window position (unused if docked)
        public SharpDX.Vector2 windowPosition = new SharpDX.Vector2(0.0f);

        // `windowPosCondition`: _ImGuiCond, default=ImGuiCond_FirstUseEver_.
        //  When to apply the window position.
        public ImGuiCond windowPositionCondition = ImGuiCond.FirstUseEver;
    }

    public enum DockingLayoutCondition
    {
        FirstUseEver,
        ApplicationStart,
        Never
    };

    public class DockingParams
    {
        // --------------- Main params -----------------------------

        // `dockingSplits`: _vector[DockingSplit]_.
        //  Defines the way docking splits should be applied on the screen
        //  in order to create new Dock Spaces
        public List<DockingSplit> dockingSplits = new List<DockingSplit>();

        // `dockableWindows`: _vector[DockableWindow]_.
        //  List of the dockable windows, together with their Gui code
        public List<DockableWindow> dockableWindows = new List<DockableWindow>();

        // `layoutName`: _string, default="default"_.
        //  Displayed name of the layout.
        //  Only used in advanced cases, when several layouts are available.
        public string layoutName = "Default";

        // --------------- Options -----------------------------

        // `mainDockSpaceNodeFlags`: _ImGuiDockNodeFlags (enum),
        //      default=ImGuiDockNodeFlags_PassthruCentralNode_
        //  Flags to apply to the main dock space
        //  (enable/disable resizing, splitting, tab bar, etc.).
        //  Most flags are inherited by children dock spaces.
        //  You can also set flags for specific dock spaces via
        //  `DockingSplit.nodeFlags`
        public ImGuiDockNodeFlags mainDockSpaceNodeFlags = ImGuiDockNodeFlags.PassthruCentralNode;

        // --------------- Layout handling -----------------------------

        // `layoutCondition`: _enum DockingLayoutCondition, default=FirstUseEver_.
        //  When to apply the docking layout. Choose between
        //      FirstUseEver (apply once, then keep user preference),
        //      ApplicationStart (always reapply at application start)
        //      Never
        public DockingLayoutCondition layoutCondition = DockingLayoutCondition.FirstUseEver;

        // `layoutReset`: _bool, default=false_.
        //  Reset layout on next frame, i.e. drop the layout customizations which
        //  were applied manually by the user. layoutReset will be reset to false
        //  after this.
        public bool layoutReset = true;

        // --------------- Helper Methods -----------------------------

        // `DockableWindow * dockableWindowOfName(const std::string & name)`:
        // returns a pointer to a dockable window
        public DockableWindow DockableWindowOfName(string name)
        {
            return new DockableWindow();
        }

	    // `bool focusDockableWindow(const std::string& name)`:
	    // will focus a dockable window (and make its tab visible if needed)
        public bool FocusDockableWindow(string windowName)
        {
            DockableWindow win = DockableWindowOfName(windowName);
            if (win != null)
            {
                win.focusWindowAtNextFrame = true;
                return true;
            }
            else
            {
                return false;
            }
        }

	    // `optional<ImGuiID> dockSpaceIdFromName(const std::string&
	    // dockSpaceName)`: returns the ImGuiID corresponding to the dockspace with
	    // this name
	    public uint? DockSpaceIdFromName(string dockSpaceName)
        {
            if (SplitHelper.ContainsSplit(dockSpaceName))
                return SplitHelper.GetSplitID(dockSpaceName);
            else
                return null;
        }
};
}