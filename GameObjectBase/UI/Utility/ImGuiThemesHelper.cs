using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase.UI.Utility
{
    public static class ImGuiThemesHelper
    {
        public enum ImGuiTheme
        {
            ImGuiTheme_ImGuiColorsClassic = 0,
            ImGuiTheme_ImGuiColorsDark,
            ImGuiTheme_SoDark_AccentRed
        };

        public class ThemeInfo
        {
            public ImGuiTheme Theme;
            public string Name;
            public ImGuiStylePtr Style;

            public ThemeInfo() { }
            public ThemeInfo(ImGuiTheme theme, string name, ImGuiStylePtr style)
            {
                Theme = theme;
                Name = name;
                Style = style;
            }
        };
        static void ApplyRounding(ImGuiStyle ioStyle, float rounding, float scrollbarRatio)
        {
            ioStyle.WindowRounding = rounding;
            ioStyle.ChildRounding = rounding;
            ioStyle.PopupRounding = rounding;
            ioStyle.FrameRounding = rounding;
            ioStyle.GrabRounding = rounding;
            ioStyle.TabRounding = rounding;
            ioStyle.ScrollbarRounding = rounding * scrollbarRatio;
        }

        static void ApplyAlphaMultiplier(ImGuiStyle ioStyle, float alphaMultiplier, ImGuiStyle referenceStyle)
        {
            for (int i = 0; i < (int)ImGuiCol.COUNT; ++i)
            {
                ImGuiCol colEnum = (ImGuiCol)i;
                SharpDX.Vector4 col = (SharpDX.Vector4)typeof(ImGuiStyle).GetField("Count_" + i).GetValue(ioStyle);
                SharpDX.Vector4 colRef = (SharpDX.Vector4)typeof(ImGuiStyle).GetField("Count_" + i).GetValue(referenceStyle);
                bool apply = false;
                {
                    bool isWindowBackground = (colEnum == ImGuiCol.FrameBg) ||
                                              (colEnum == ImGuiCol.WindowBg) ||
                                              (colEnum == ImGuiCol.ChildBg);
                    bool isAlreadyTransparent = col.W < 1.00f;
                    bool isDimColor = (colEnum == ImGuiCol.ModalWindowDimBg) ||
                                      (colEnum == ImGuiCol.NavWindowingDimBg);
                    if (!isDimColor && (isAlreadyTransparent || isWindowBackground))
                    {
                        apply = true;
                    }
                }
                if (apply)
                {
                    col.W = alphaMultiplier * colRef.W;
                    if (col.W < 0.0f)
                        col.W = 0.0f;
                    if (col.W > 1.0f)
                        col.W = 1.0f;
                }
            }
        }

        static void ApplySaturationMultiplier(ImGuiStyle ioStyle, float saturationMultiplier, ImGuiStyle referenceStyle)
        {
            for (int i = 0; i < (int)ImGuiCol.COUNT; ++i)
            {
                SharpDX.Vector4 col = (SharpDX.Vector4)typeof(ImGuiStyle).GetField("Count_" + i).GetValue(ioStyle);
                SharpDX.Vector4 colRef = (SharpDX.Vector4)typeof(ImGuiStyle).GetField("Count_" + i).GetValue(referenceStyle);

                float h, s, v;
                ImGui.ColorConvertRGBtoHSV(col.X, col.Y, col.Z, out h, out s, out v);
                float h_ref, s_ref, v_ref;
                ImGui.ColorConvertRGBtoHSV(colRef.X, colRef.Y, colRef.Z, out h_ref, out s_ref, out v_ref);
                s = s_ref * saturationMultiplier;
                ImGui.ColorConvertHSVtoRGB(h, s, v, out col.X, out col.Y, out col.Z);
            }
        }

        static ImGuiCol[] gBgColors = { ImGuiCol.WindowBg, ImGuiCol.ChildBg, ImGuiCol.PopupBg };
        static ImGuiCol[] gTextColors = { ImGuiCol.Text, ImGuiCol.TextDisabled };

        enum ColorCategory
        {
            ColorBg,
            ColorFront,
            ColorText,
            ColorFrameBg
        };

        static ColorCategory GetColorCategory(ImGuiCol col)
        {
            if (col == ImGuiCol.FrameBg)
                return ColorCategory.ColorFrameBg;
            for (int i = 0; i < gBgColors.Length; ++i)
                if (col == gBgColors[i])
                    return ColorCategory.ColorBg;
            for (int i = 0; i < gTextColors.Length; ++i)
                if (col == gTextColors[i])
                    return ColorCategory.ColorText;
            return ColorCategory.ColorFront;
        }

        static SharpDX.Vector4 ColorValueMultiply(SharpDX.Vector4 col, float valueMultiplier)
        {
            float h, s, v;
            ImGui.ColorConvertRGBtoHSV(col.X, col.Y, col.Z, out h, out s, out v);
            v = v * valueMultiplier;
            SharpDX.Vector4 r = col;
            ImGui.ColorConvertHSVtoRGB(h, s, v, out r.X, out r.Y, out r.Z);
            return r;
        }

        static SharpDX.Vector4 ColorSetValue(SharpDX.Vector4 col, float value)
        {
            float h, s, v;
            ImGui.ColorConvertRGBtoHSV(col.X, col.Y, col.Z, out h, out s, out v);
            v = value;
            SharpDX.Vector4 r = col;
            ImGui.ColorConvertHSVtoRGB(h, s, v, out r.X, out r.Y, out r.Z);
            return r;
        }

        static void ApplyValueMultiplier(ImGuiStyle style, float valueMultiplier, ImGuiStyle referenceStyle, ColorCategory category)
        {
            for (int i = 0; i < (int)ImGuiCol.COUNT; ++i)
            {
                ImGuiCol colEnum = (ImGuiCol)i;
                if (GetColorCategory(colEnum) == category)
                {
                    SharpDX.Vector4 col = (SharpDX.Vector4)typeof(ImGuiStyle).GetField("Count_" + i).GetValue(style);
                    SharpDX.Vector4 colRef = (SharpDX.Vector4)typeof(ImGuiStyle).GetField("Count_" + i).GetValue(referenceStyle);

                    float h, s, v;
                    ImGui.ColorConvertRGBtoHSV(col.X, col.Y, col.Z, out h, out s, out v);
                    float h_ref, s_ref, v_ref;
                    ImGui.ColorConvertRGBtoHSV(colRef.X, colRef.Y, colRef.Z, out h_ref, out s_ref, out v_ref);
                    v = v_ref * valueMultiplier;
                    ImGui.ColorConvertHSVtoRGB(h, s, v, out col.X, out col.Y, out col.Z);
                }
            }
        }

        static void ApplyValueMultiplierFront(ImGuiStyle style, float valueMultiplier, ImGuiStyle referenceStyle)
        {
            ApplyValueMultiplier(style, valueMultiplier, referenceStyle, ColorCategory.ColorFront);
        }

        static void ApplyValueMultiplierBg(ImGuiStyle style, float valueMultiplier, ImGuiStyle referenceStyle)
        {
            ApplyValueMultiplier(style, valueMultiplier, referenceStyle, ColorCategory.ColorBg);
        }

        static void ApplyValueMultiplierText(ImGuiStyle style, float valueMultiplier, ImGuiStyle referenceStyle)
        {
            ApplyValueMultiplier(style, valueMultiplier, referenceStyle, ColorCategory.ColorText);
        }

        static void ApplyValueMultiplierFrameBg(ImGuiStyle style, float valueMultiplier, ImGuiStyle referenceStyle)
        {
            ApplyValueMultiplier(style, valueMultiplier, referenceStyle, ColorCategory.ColorFrameBg);
        }

        static void ApplyHue(ImGuiStylePtr style, float hue)
        {
            for (int i = 0; i < (int)ImGuiCol.COUNT; i++)
            {
                SharpDX.Vector4 col = style.Colors[i];

                float h, s, v;
                ImGui.ColorConvertRGBtoHSV(col.X, col.Y, col.Z, out h, out s, out v);
                h = hue;
                ImGui.ColorConvertHSVtoRGB(h, s, v, out col.X, out col.Y, out col.Z);
            }
        }

        static ImGuiStylePtr ImGui_StyleColorsClassic()
        {
            ImGuiStylePtr style;
            ImGui.StyleColorsClassic(style);
            return style;
        }

        static ImGuiStylePtr ImGui_StyleColorsDark()
        {
            ImGuiStylePtr style;
            ImGui.StyleColorsDark(style);
            return style;
        }

        static ImGuiStylePtr SoDark(float hue)
        {
            ImGuiStylePtr style;
            var colors = style.Colors;
            colors[(int)ImGuiCol.Text] = new SharpDX.Vector4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[(int)ImGuiCol.TextDisabled] = new SharpDX.Vector4(0.50f, 0.50f, 0.50f, 1.00f);
            colors[(int)ImGuiCol.WindowBg] = new SharpDX.Vector4(0.10f, 0.10f, 0.10f, 1.00f);
            colors[(int)ImGuiCol.ChildBg] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[(int)ImGuiCol.PopupBg] = new SharpDX.Vector4(0.19f, 0.19f, 0.19f, 0.92f);
            colors[(int)ImGuiCol.Border] = new SharpDX.Vector4(0.39f, 0.39f, 0.39f, 0.59f);
            colors[(int)ImGuiCol.BorderShadow] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 0.24f);
            colors[(int)ImGuiCol.FrameBg] = new SharpDX.Vector4(0.25f, 0.25f, 0.25f, 0.54f);
            colors[(int)ImGuiCol.FrameBgHovered] = new SharpDX.Vector4(0.19f, 0.19f, 0.19f, 0.54f);
            colors[(int)ImGuiCol.FrameBgActive] = new SharpDX.Vector4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[(int)ImGuiCol.TitleBg] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 1.00f);
            colors[(int)ImGuiCol.TitleBgActive] = new SharpDX.Vector4(0.06f, 0.06f, 0.06f, 1.00f);
            colors[(int)ImGuiCol.TitleBgCollapsed] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 1.00f);
            colors[(int)ImGuiCol.MenuBarBg] = new SharpDX.Vector4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[(int)ImGuiCol.ScrollbarBg] = new SharpDX.Vector4(0.05f, 0.05f, 0.05f, 0.54f);
            colors[(int)ImGuiCol.ScrollbarGrab] = new SharpDX.Vector4(0.34f, 0.34f, 0.34f, 0.54f);
            colors[(int)ImGuiCol.ScrollbarGrabHovered] = new SharpDX.Vector4(0.40f, 0.40f, 0.40f, 0.54f);
            colors[(int)ImGuiCol.ScrollbarGrabActive] = new SharpDX.Vector4(0.56f, 0.56f, 0.56f, 0.54f);
            colors[(int)ImGuiCol.CheckMark] = new SharpDX.Vector4(0.33f, 0.67f, 0.86f, 1.00f);
            colors[(int)ImGuiCol.SliderGrab] = new SharpDX.Vector4(0.34f, 0.34f, 0.34f, 0.54f);
            colors[(int)ImGuiCol.SliderGrabActive] = new SharpDX.Vector4(0.56f, 0.56f, 0.56f, 0.54f);
            colors[(int)ImGuiCol.Button] = new SharpDX.Vector4(0.30f, 0.30f, 0.30f, 0.54f);
            colors[(int)ImGuiCol.ButtonHovered] = new SharpDX.Vector4(0.19f, 0.19f, 0.19f, 0.54f);
            colors[(int)ImGuiCol.ButtonActive] = new SharpDX.Vector4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[(int)ImGuiCol.Header] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 0.01f);
            colors[(int)ImGuiCol.HeaderHovered] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 0.36f);
            colors[(int)ImGuiCol.HeaderActive] = new SharpDX.Vector4(0.20f, 0.22f, 0.23f, 0.33f);
            colors[(int)ImGuiCol.Separator] = new SharpDX.Vector4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[(int)ImGuiCol.SeparatorHovered] = new SharpDX.Vector4(0.44f, 0.44f, 0.44f, 0.29f);
            colors[(int)ImGuiCol.SeparatorActive] = new SharpDX.Vector4(0.40f, 0.44f, 0.47f, 1.00f);
            colors[(int)ImGuiCol.ResizeGrip] = new SharpDX.Vector4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[(int)ImGuiCol.ResizeGripHovered] = new SharpDX.Vector4(0.44f, 0.44f, 0.44f, 0.29f);
            colors[(int)ImGuiCol.ResizeGripActive] = new SharpDX.Vector4(0.40f, 0.44f, 0.47f, 1.00f);
            colors[(int)ImGuiCol.Tab] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[(int)ImGuiCol.TabHovered] = new SharpDX.Vector4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[(int)ImGuiCol.TabActive] = new SharpDX.Vector4(0.20f, 0.20f, 0.20f, 0.36f);
            colors[(int)ImGuiCol.TabUnfocused] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[(int)ImGuiCol.TabUnfocusedActive] = new SharpDX.Vector4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[(int)ImGuiCol.DockingPreview] = new SharpDX.Vector4(0.33f, 0.67f, 0.86f, 1.00f);
            colors[(int)ImGuiCol.DockingEmptyBg] = new SharpDX.Vector4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[(int)ImGuiCol.PlotLines] = new SharpDX.Vector4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[(int)ImGuiCol.PlotLinesHovered] = new SharpDX.Vector4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[(int)ImGuiCol.PlotHistogram] = new SharpDX.Vector4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[(int)ImGuiCol.PlotHistogramHovered] = new SharpDX.Vector4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[(int)ImGuiCol.TableHeaderBg] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[(int)ImGuiCol.TableBorderStrong] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 0.52f);
            colors[(int)ImGuiCol.TableBorderLight] = new SharpDX.Vector4(0.28f, 0.28f, 0.28f, 0.29f);
            colors[(int)ImGuiCol.TableRowBg] = new SharpDX.Vector4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[(int)ImGuiCol.TableRowBgAlt] = new SharpDX.Vector4(1.00f, 1.00f, 1.00f, 0.06f);
            colors[(int)ImGuiCol.TextSelectedBg] = new SharpDX.Vector4(0.20f, 0.22f, 0.23f, 1.00f);
            colors[(int)ImGuiCol.DragDropTarget] = new SharpDX.Vector4(0.33f, 0.67f, 0.86f, 1.00f);
            colors[(int)ImGuiCol.NavHighlight] = new SharpDX.Vector4(1.00f, 0.00f, 0.00f, 1.00f);
            colors[(int)ImGuiCol.NavWindowingHighlight] = new SharpDX.Vector4(1.00f, 0.00f, 0.00f, 0.70f);
            colors[(int)ImGuiCol.NavWindowingDimBg] = new SharpDX.Vector4(1.00f, 0.00f, 0.00f, 0.20f);
            colors[(int)ImGuiCol.ModalWindowDimBg] = new SharpDX.Vector4(1.00f, 0.00f, 0.00f, 0.35f);

            style.WindowPadding = new SharpDX.Vector2(8.00f, 8.00f);
            style.FramePadding = new SharpDX.Vector2(5.00f, 5.00f);
            style.CellPadding = new SharpDX.Vector2(5.00f, 5.00f);
            style.ItemSpacing = new SharpDX.Vector2(5.00f, 5.00f);
            style.ItemInnerSpacing = new SharpDX.Vector2(5.00f, 5.00f);
            style.TouchExtraPadding = new SharpDX.Vector2(0.00f, 0.00f);
            style.IndentSpacing = 5;
            style.ScrollbarSize = 15;
            style.GrabMinSize = 10;
            style.WindowBorderSize = 1;
            style.ChildBorderSize = 1;
            style.PopupBorderSize = 1;
            style.FrameBorderSize = 0;
            style.TabBorderSize = 1;
            style.WindowRounding = 0;
            style.ChildRounding = 0;
            style.FrameRounding = 0;
            style.PopupRounding = 0;
            style.ScrollbarRounding = 9;
            style.GrabRounding = 3;
            style.LogSliderDeadzone = 4;
            style.TabRounding = 0;

            ApplyHue(style, hue);

            return style;
        }

        static ThemeInfo[] gThemeInfos = {
            new ThemeInfo(ImGuiTheme.ImGuiTheme_ImGuiColorsClassic, "ImGuiColorsClassic", ImGui_StyleColorsClassic()),
            new ThemeInfo(ImGuiTheme.ImGuiTheme_ImGuiColorsDark, "ImGuiColorsDark", ImGui_StyleColorsDark()),
            new ThemeInfo(ImGuiTheme.ImGuiTheme_SoDark_AccentRed, "SoDark_AccentRed", SoDark(0.0f)) 
        };

        static string ImGuiTheme_Name(ImGuiTheme theme)
        {
            for (int i = 0; i < gThemeInfos.Count(); ++i)
            {
                if (gThemeInfos[i].Theme == theme)
                    return gThemeInfos[i].Name;
            }
            return "";
        }

        static ImGuiTheme ImGuiTheme_FromName(string themeName)
        {

            string themeStr = themeName;
	        for (int i = 0; i < gThemeInfos.Count(); ++i)
	        {
		        if (gThemeInfos[i].Name == themeStr)
			        return gThemeInfos[i].Theme;
	        }
	        return ImGuiTheme.ImGuiTheme_ImGuiColorsClassic;
        }

        static ImGuiStylePtr ThemeToStyle(ImGuiTheme theme)
        {
            for (int i = 0; i < gThemeInfos.Count(); i++)
            {
                if (gThemeInfos[i].Theme == theme)
                    return gThemeInfos[i].Style;
            }
            return new ImGuiStylePtr();
        }

        static unsafe void ApplyTheme(ImGuiTheme theme)
        {
            ImGuiStylePtr style = ThemeToStyle(theme);
            //Note: Fix style later
            //ImGui.GetStyle() = style;
        }
    }
}
