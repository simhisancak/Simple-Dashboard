#include "Sidebar.h"

#include <imgui.h>
#include <windows.h>

#include "UI/Theme.h"
#include "Utils/Injector/Injector.h"

namespace UI {
namespace Views {

DashboardSidebar::DashboardSidebar(Application* app) : m_App(app) {}

void DashboardSidebar::Render(int& selectedPid) {
    // Header bölümü - Sabit, scroll olmayan
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ThemeConfig::BgMid());
    ImGui::BeginChild("SidebarHeader",
                      ImVec2(0, ThemeConfig::SidebarHeaderHeight),
                      false,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Connected Apps'i yatay ve dikey olarak ortala
    float textHeight = ImGui::GetTextLineHeight();
    float headerHeight = ThemeConfig::SidebarHeaderHeight;
    float windowPadding = ThemeConfig::WindowPadding;
    float availableHeight = headerHeight - (windowPadding * 2.0f);
    float yPos = windowPadding + (availableHeight - textHeight) * 0.5f;

    // Yatay ortalama için hesaplamalar
    float windowWidth = ImGui::GetWindowWidth();
    float textWidth = ImGui::CalcTextSize("Connected Apps").x;
    float xPos = (windowWidth - textWidth) * 0.5f;

    ImGui::SetCursorPos(ImVec2(xPos, yPos));
    ImGui::TextUnformatted("Connected Apps");

    // Separator'ı header'ın en altına yerleştir
    ImGui::SetCursorPosY(headerHeight - ThemeConfig::SeparatorPadding);
    ThemeConfig::DrawSeparator(false, 10.0f);

    ImGui::EndChild();
    ImGui::PopStyleColor();

    // Content bölümü - Sadece bu kısım scroll olabilir
    ImGui::BeginChild("SidebarContent",
                      ImVec2(0,
                             -ThemeConfig::SidebarFooterHeight),  // Footer yüksekliği kadar çıkar
                      false);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ThemeConfig::ItemSpacing, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                        ImVec2(ThemeConfig::ViewPadding, ThemeConfig::FramePadding));
    // Text'i ortala
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

    const auto& clients = m_App->GetServerManager()->GetConnectedClients();
    for (const auto& client : clients) {
        bool isSelected = (selectedPid == client->pid);
        std::string label = client->processName + " (PID: " + std::to_string(client->pid) + ")";

        if (ImGui::Selectable(label.c_str(), isSelected)) {
            selectedPid = client->pid;
        }

        if (isSelected) {
            ImGui::SetItemDefaultFocus();
        }
    }

    ImGui::PopStyleVar(3);
    ImGui::EndChild();
}

}  // namespace Views
}  // namespace UI