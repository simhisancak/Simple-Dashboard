#include "MainContent.h"
#include "FarmBot/FarmBot.h"
#include "Main/Main.h"
#include "UI/Theme.h"
#include <imgui.h>
#include <memory>

namespace UI {
namespace Views {

    DashboardMainContent::DashboardMainContent(Application* app) {
        m_Contents.push_back(std::make_unique<Main>(app));
        m_Contents.push_back(std::make_unique<FarmBot>(app));
    }

    void DashboardMainContent::Render(const DashboardContent* activeContent, int selectedPid) {
        if (!activeContent) {
            // Center the text using theme padding
            float textWidth = ImGui::CalcTextSize("Select a form from the header menu").x;
            float textHeight = ImGui::GetTextLineHeight();
            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowWidth() - textWidth) * 0.5f,
                                       (ImGui::GetWindowHeight() - textHeight) * 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Text, ThemeConfig::TextSecondary());
            ImGui::Text("Select a form from the header menu");
            ImGui::PopStyleColor();
            return;
        }

        for (const auto& content : m_Contents) {
            if (content->IsSameContent(activeContent)) {
                content->Render(selectedPid);
                break;
            }
        }
    }
}
}