#include "ImguiThemes.h"
#include <magic_enum/magic_enum.hpp>

namespace ThemeTweakImpl
{
	void ApplyRounding(ImGuiStyle& ioStyle, float rounding, float scrollbarRatio)
	{
		ioStyle.WindowRounding = rounding;
		ioStyle.ChildRounding = rounding;
		ioStyle.PopupRounding = rounding;
		ioStyle.FrameRounding = rounding;
		ioStyle.GrabRounding = rounding;
		ioStyle.TabRounding = rounding;
		ioStyle.ScrollbarRounding = rounding * scrollbarRatio;
	}

	void ApplyAlphaMultiplier(ImGuiStyle& ioStyle, float alphaMultiplier, ImGuiStyle& referenceStyle)
	{
		for (int i = 0; i < ImGuiCol_COUNT; ++i)
		{
			ImGuiCol_ colEnum = (ImGuiCol_)i;
			ImVec4& col = ioStyle.Colors[i];
			const ImVec4& colRef = referenceStyle.Colors[i];
			bool apply = false;
			{
				bool isWindowBackground = (colEnum == ImGuiCol_FrameBg) ||
				                          (colEnum == ImGuiCol_WindowBg) ||
				                          (colEnum == ImGuiCol_ChildBg);
				bool isAlreadyTransparent = col.w < 1.00f;
				bool isDimColor = (colEnum == ImGuiCol_ModalWindowDimBg) ||
				                  (colEnum == ImGuiCol_NavWindowingDimBg);
				if (!isDimColor && (isAlreadyTransparent || isWindowBackground))
				{
					apply = true;
				}
			}
			if (apply)
			{
				col.w = alphaMultiplier * colRef.w;
				if (col.w < 0.)
					col.w = 0.;
				if (col.w > 1.f)
					col.w = 1.f;
			}
		}
	}

	void ApplySaturationMultiplier(ImGuiStyle& ioStyle, float saturationMultiplier, ImGuiStyle& referenceStyle)
	{
		for (int i = 0; i < ImGuiCol_COUNT; ++i)
		{
			ImVec4& col = ioStyle.Colors[i];
			const ImVec4& colRef = referenceStyle.Colors[i];

			float h, s, v;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, h, s, v);
			float h_ref, s_ref, v_ref;
			ImGui::ColorConvertRGBtoHSV(colRef.x, colRef.y, colRef.z, h_ref,
			                            s_ref, v_ref);
			s = s_ref * saturationMultiplier;
			ImGui::ColorConvertHSVtoRGB(h, s, v, col.x, col.y, col.z);
		}
	}

	ImGuiCol_ gBgColors[] = {ImGuiCol_WindowBg, ImGuiCol_ChildBg,
	                         ImGuiCol_PopupBg};

	ImGuiCol_ gTextColors[] = {
		ImGuiCol_Text,
		ImGuiCol_TextDisabled,
	};

	enum class ColorCategory
	{
		ColorBg,
		ColorFront,
		ColorText,
		ColorFrameBg
	};

	ColorCategory GetColorCategory(ImGuiCol_ col)
	{
		if (col == ImGuiCol_FrameBg)
			return ColorCategory::ColorFrameBg;
		for (size_t i = 0; i < IM_ARRAYSIZE(gBgColors); ++i)
			if (col == gBgColors[i])
				return ColorCategory::ColorBg;
		for (size_t i = 0; i < IM_ARRAYSIZE(gTextColors); ++i)
			if (col == gTextColors[i])
				return ColorCategory::ColorText;
		return ColorCategory::ColorFront;
	}

	static ImVec4 ColorValueMultiply(ImVec4 col, float valueMultiplier)
	{
		float h, s, v;
		ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, h, s, v);
		v = v * valueMultiplier;
		ImVec4 r = col;
		ImGui::ColorConvertHSVtoRGB(h, s, v, r.x, r.y, r.z);
		return r;
	}

	static ImVec4 ColorSetValue(ImVec4 col, float value)
	{
		float h, s, v;
		ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, h, s, v);
		v = value;
		ImVec4 r = col;
		ImGui::ColorConvertHSVtoRGB(h, s, v, r.x, r.y, r.z);
		return r;
	}

	void ApplyValueMultiplier(ImGuiStyle& style, float valueMultiplier, ImGuiStyle& referenceStyle, ColorCategory category)
	{
		for (int i = 0; i < ImGuiCol_COUNT; ++i)
		{
			ImGuiCol_ colEnum = (ImGuiCol_)i;
			if (GetColorCategory(colEnum) == category)
			{
				ImVec4& col = style.Colors[i];
				const ImVec4& colRef = referenceStyle.Colors[i];

				float h, s, v;
				ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, h, s, v);
				float h_ref, s_ref, v_ref;
				ImGui::ColorConvertRGBtoHSV(colRef.x, colRef.y, colRef.z, h_ref,
				                            s_ref, v_ref);
				v = v_ref * valueMultiplier;
				ImGui::ColorConvertHSVtoRGB(h, s, v, col.x, col.y, col.z);
			}
		}
	}

	void ApplyValueMultiplierFront(ImGuiStyle& style, float valueMultiplier, ImGuiStyle& referenceStyle)
	{
		ApplyValueMultiplier(style, valueMultiplier, referenceStyle, ColorCategory::ColorFront);
	}

	void ApplyValueMultiplierBg(ImGuiStyle& style, float valueMultiplier, ImGuiStyle& referenceStyle)
	{
		ApplyValueMultiplier(style, valueMultiplier, referenceStyle, ColorCategory::ColorBg);
	}

	void ApplyValueMultiplierText(ImGuiStyle& style, float valueMultiplier, ImGuiStyle& referenceStyle)
	{
		ApplyValueMultiplier(style, valueMultiplier, referenceStyle, ColorCategory::ColorText);
	}

	void ApplyValueMultiplierFrameBg(ImGuiStyle& style, float valueMultiplier, ImGuiStyle& referenceStyle)
	{
		ApplyValueMultiplier(style, valueMultiplier, referenceStyle, ColorCategory::ColorFrameBg);
	}

	void ApplyHue(ImGuiStyle& style, float hue)
	{
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			ImVec4& col = style.Colors[i];

			float h, s, v;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, h, s, v);
			h = hue;
			ImGui::ColorConvertHSVtoRGB(h, s, v, col.x, col.y, col.z);
		}
	}

} // namespace ThemeTweakImpl

namespace ThemesImpl
{
	ImGuiStyle ImGui_StyleColorsClassic()
	{
		ImGuiStyle style;
		ImGui::StyleColorsClassic(&style);
		return style;
	}

	ImGuiStyle ImGui_StyleColorsDark()
	{
		ImGuiStyle style;
		ImGui::StyleColorsDark(&style);
		return style;
	}

	ImGuiStyle SoDark(float hue)
	{
		ImGuiStyle style;
		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.39f, 0.39f, 0.39f, 0.59f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.30f, 0.30f, 0.30f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.01f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 5.00f);
		style.CellPadding = ImVec2(5.00f, 5.00f);
		style.ItemSpacing = ImVec2(5.00f, 5.00f);
		style.ItemInnerSpacing = ImVec2(5.00f, 5.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
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

		ThemeTweakImpl::ApplyHue(style, hue);

		return style;
	}
} // namespace ThemesImpl

ThemeInfo gThemeInfos[] = {
	{ImGuiTheme_ImGuiColorsClassic, "ImGuiColorsClassic",
     ThemesImpl::ImGui_StyleColorsClassic()},
	{ImGuiTheme_ImGuiColorsDark, "ImGuiColorsDark",
     ThemesImpl::ImGui_StyleColorsDark()},
	{ImGuiTheme_SoDark_AccentRed, "SoDark_AccentRed", ThemesImpl::SoDark(0.f)}};

const char* ImGuiTheme_Name(ImGuiTheme theme)
{
	IM_ASSERT((theme >= 0) && (theme < magic_enum::enum_count<ImGuiTheme>()));
	for (size_t i = 0; i < IM_ARRAYSIZE(gThemeInfos); ++i)
	{
		if (gThemeInfos[i].theme == theme)
			return gThemeInfos[i].Name;
	}
	return "";
}

ImGuiTheme ImGuiTheme_FromName(const char* themeName)
{
	std::string themeStr = themeName;
	for (size_t i = 0; i < IM_ARRAYSIZE(gThemeInfos); ++i)
	{
		if (gThemeInfos[i].Name == themeStr)
			return gThemeInfos[i].theme;
	}
	return ImGuiTheme_ImGuiColorsClassic;
}

ImGuiStyle ThemeToStyle(ImGuiTheme theme)
{
	IM_ASSERT((theme >= 0) && (theme < magic_enum::enum_count<ImGuiTheme>()));
	for (size_t i = 0; IM_ARRAYSIZE(gThemeInfos); i++)
	{
		if (gThemeInfos[i].theme == theme)
			return gThemeInfos[i].Style;
	}
	return ImGuiStyle();
}

void ApplyTheme(ImGuiTheme theme)
{
	ImGuiStyle style = ThemeToStyle(theme);
	ImGui::GetStyle() = style;
}
