#pragma once
#include <imgui.h>

namespace UI {

struct ThemeConfig {
    // Genel Ayarlar
    static constexpr float WindowPadding = 10.0f;
    static constexpr float FramePadding = 8.0f;
    static constexpr float ItemSpacing = 8.0f;
    static constexpr float ScrollbarSize = 12.0f;
    static constexpr float WindowBorderSize = 1.0f;
    static constexpr float ChildBorderSize = 1.0f;
    static constexpr float PopupBorderSize = 1.0f;
    static constexpr float FrameBorderSize = 1.0f;
    static constexpr float WindowRounding = 4.0f;
    static constexpr float ButtonPadding = 6.0f;

    // Layout Sabitleri
    static constexpr float ViewPadding = 12.0f;
    static constexpr float SidebarWidth = 220.0f;
    static constexpr float SeparatorPadding = 3.0f;
    static constexpr float HeaderHeight = 40.0f;
    static constexpr float TitleBarHeight = 32.0f;
    static constexpr float SidebarHeaderHeight = 40.0f;
    static constexpr float SidebarFooterHeight = 60.0f;
    static constexpr float AddButtonSize = 30.0f;

    // Otomatik hesaplanan değerler
    static void DrawSeparator(bool isVertical = false, float size = 0.9f)
    {
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 startPos, endPos;

        if (!isVertical) {
            // Yatay separator - ortala
            float width;
            if (size <= 0.0f) {
                width = contentSize.x * 0.9f; // Varsayılan %90
            } else if (size <= 1.0f) {
                width = contentSize.x * size; // Yüzde olarak
            } else {
                width = contentSize.x - size; // Piksel olarak
            }

            float startX = cursorPos.x + (contentSize.x - width) * 0.5f;
            startPos = ImVec2(startX, cursorPos.y);
            endPos = ImVec2(startX + width, cursorPos.y);
        } else {
            // Dikey separator
            float height;
            if (size <= 0.0f) {
                height = contentSize.y * 0.9f; // Varsayılan %90
            } else if (size <= 1.0f) {
                height = contentSize.y * size; // Yüzde olarak
            } else {
                height = contentSize.y - size; // Piksel olarak
            }

            float startY = cursorPos.y + (contentSize.y - height) * 0.5f;
            startPos = ImVec2(cursorPos.x, startY);
            endPos = ImVec2(cursorPos.x, startY + height);
        }

        ImGui::GetWindowDrawList()->AddLine(startPos,
                                            endPos,
                                            ImGui::GetColorU32(ThemeConfig::Border()),
                                            1.0f);

        // Cursor'ı ilerlet
        if (!isVertical) {
            ImGui::Dummy(ImVec2(0, 1));
        } else {
            ImGui::Dummy(ImVec2(1, 0));
        }
    }

    // Renkler
    static const ImVec4 AccentColor() { return ImVec4(0.28f, 0.56f, 0.90f, 1.00f); }
    static const ImVec4 BgDark() { return ImVec4(0.10f, 0.10f, 0.10f, 1.00f); }
    static const ImVec4 BgMid() { return ImVec4(0.15f, 0.15f, 0.15f, 1.00f); }
    static const ImVec4 BgLight() { return ImVec4(0.20f, 0.20f, 0.20f, 1.00f); }
    static const ImVec4 TextPrimary() { return ImVec4(0.90f, 0.90f, 0.90f, 1.00f); }
    static const ImVec4 TextSecondary() { return ImVec4(0.60f, 0.60f, 0.60f, 1.00f); }
    static const ImVec4 Border() { return ImVec4(0.25f, 0.25f, 0.25f, 1.00f); }
};

class Theme {
public:
    static void Apply()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Ana Renkler
        const ImVec4 accent = ImVec4(0.28f, 0.56f, 0.90f, 1.00f); // Mavi accent
        const ImVec4 bgDark = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // Koyu arka plan
        const ImVec4 bgMid = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Orta arka plan
        const ImVec4 bgLight = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // Açık arka plan
        const ImVec4 textPrimary = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        const ImVec4 textSecondary = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

        // Genel Stil ve Spacing
        style.WindowPadding = ImVec2(ThemeConfig::WindowPadding, ThemeConfig::WindowPadding);
        style.FramePadding = ImVec2(ThemeConfig::ButtonPadding,
                                    ThemeConfig::ButtonPadding); // Butonlar için eşit padding
        style.ItemSpacing = ImVec2(ThemeConfig::ItemSpacing, ThemeConfig::ItemSpacing);
        style.ScrollbarSize = ThemeConfig::ScrollbarSize;
        style.WindowBorderSize = ThemeConfig::WindowBorderSize;
        style.ChildBorderSize = ThemeConfig::ChildBorderSize;
        style.PopupBorderSize = ThemeConfig::PopupBorderSize;
        style.FrameBorderSize = ThemeConfig::FrameBorderSize;
        style.WindowRounding = ThemeConfig::WindowRounding;

        // Yuvarlak Köşeler
        style.ChildRounding = 4.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 4.0f;
        style.ScrollbarRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;

        // Hizalama
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f); // Buton içinde metni ortala
        style.SelectableTextAlign = ImVec2(0.0f, 0.5f);

        // Kenar Çizgileri
        style.TabBorderSize = 1.0f;

        // Renkler
        colors[ImGuiCol_Text] = textPrimary;
        colors[ImGuiCol_TextDisabled] = textSecondary;
        colors[ImGuiCol_WindowBg] = bgDark;
        colors[ImGuiCol_ChildBg] = bgMid;
        colors[ImGuiCol_PopupBg] = bgDark;
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = bgLight;
        colors[ImGuiCol_FrameBgHovered] = ImVec4(accent.x, accent.y, accent.z, 0.15f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(accent.x, accent.y, accent.z, 0.30f);
        colors[ImGuiCol_TitleBg] = bgDark;
        colors[ImGuiCol_TitleBgActive] = bgMid;
        colors[ImGuiCol_TitleBgCollapsed] = bgDark;
        colors[ImGuiCol_MenuBarBg] = bgMid;
        colors[ImGuiCol_ScrollbarBg] = bgMid;
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(accent.x, accent.y, accent.z, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(accent.x, accent.y, accent.z, 0.80f);
        colors[ImGuiCol_SliderGrabActive] = accent;
        colors[ImGuiCol_Button] = ImVec4(accent.x, accent.y, accent.z, 0.60f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(accent.x, accent.y, accent.z, 0.80f);
        colors[ImGuiCol_ButtonActive] = accent;
        colors[ImGuiCol_Header] = ImVec4(accent.x, accent.y, accent.z, 0.40f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(accent.x, accent.y, accent.z, 0.60f);
        colors[ImGuiCol_HeaderActive] = ImVec4(accent.x, accent.y, accent.z, 0.80f);
        colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(accent.x, accent.y, accent.z, 0.40f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(accent.x, accent.y, accent.z, 0.60f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(accent.x, accent.y, accent.z, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(accent.x, accent.y, accent.z, 0.40f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(accent.x, accent.y, accent.z, 0.60f);
        colors[ImGuiCol_Tab] = ImVec4(accent.x, accent.y, accent.z, 0.20f);
        colors[ImGuiCol_TabHovered] = ImVec4(accent.x, accent.y, accent.z, 0.60f);
        colors[ImGuiCol_TabActive] = ImVec4(accent.x, accent.y, accent.z, 0.40f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.36f, 0.36f, 0.36f, 0.54f);
        colors[ImGuiCol_PlotLines] = accent;
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(accent.x, accent.y, accent.z, 0.80f);
        colors[ImGuiCol_PlotHistogram] = accent;
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(accent.x, accent.y, accent.z, 0.80f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.20f);
        colors[ImGuiCol_DragDropTarget] = accent;
        colors[ImGuiCol_NavHighlight] = ImVec4(accent.x, accent.y, accent.z, 0.80f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    }
};

}