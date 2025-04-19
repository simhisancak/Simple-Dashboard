#include "Header.h"

#include <imgui.h>

namespace UI {
namespace Views {

DashboardHeader::DashboardHeader() {}

void DashboardHeader::Render(const std::vector<std::unique_ptr<DashboardContent>>& contents,
                             const DashboardContent*& activeContent) {
    RenderNavigationButtons(contents, activeContent);
}

void DashboardHeader::RenderNavigationButtons(
    const std::vector<std::unique_ptr<DashboardContent>>& contents,
    const DashboardContent*& activeContent) {
    float windowWidth = ImGui::GetWindowWidth();
    float windowHeight = ImGui::GetWindowHeight();
    float buttonWidth = 140.0f;
    float totalWidth = (contents.size() * buttonWidth)
                       + ((contents.size() - 1) * ImGui::GetStyle().ItemSpacing.x);
    float startX = (windowWidth - totalWidth) * 0.5f;
    float startY = windowHeight * 0.5f - ImGui::GetFrameHeight() * 0.5f - 1.0f;

    ImGui::SetCursorPos(ImVec2(startX, startY));

    for (size_t i = 0; i < contents.size(); ++i) {
        const auto& content = contents[i];
        bool isActive = activeContent && activeContent->IsSameContent(content.get());

        if (ImGui::Button(content->GetTitle(), ImVec2(buttonWidth, 0.0f))) {
            activeContent = content.get();
        }

        if (i < contents.size() - 1) {
            ImGui::SameLine();
        }
    }
}
}  // namespace Views
}  // namespace UI