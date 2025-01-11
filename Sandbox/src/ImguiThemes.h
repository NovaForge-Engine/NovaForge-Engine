#pragma once
#include <imgui.h>


enum ImGuiTheme {
	ImGuiTheme_ImGuiColorsClassic = 0,
	ImGuiTheme_ImGuiColorsDark,
	ImGuiTheme_SoDark_AccentRed
};

struct ThemeInfo
{
	ImGuiTheme theme;
	char Name[256];
	ImGuiStyle Style;
};

const char* ImGuiTheme_Name(ImGuiTheme theme);
ImGuiTheme ImGuiTheme_FromName(const char* themeName);
ImGuiStyle ThemeToStyle(ImGuiTheme theme);
void ApplyTheme(ImGuiTheme theme);