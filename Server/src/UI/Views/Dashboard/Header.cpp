#include "Header.h"
#include <imgui.h>

namespace UI {
namespace Views {

DashboardHeader::DashboardHeader()
{
}

void DashboardHeader::Render(const std::vector<std::unique_ptr<DashboardContent>>& contents, const DashboardContent*& activeContent)
{
    RenderNavigationButtons(contents, activeContent);
}

void DashboardHeader::RenderNavigationButtons(const std::vector<std::unique_ptr<DashboardContent>>& contents, const DashboardContent*& activeContent)
{
    float windowWidth = ImGui::GetWindowWidth();
    float buttonWidth = 140.0f;
    float spacing = 20.0f;
    float totalWidth = (contents.size() * buttonWidth) + ((contents.size() - 1) * spacing);
    float startX = (windowWidth - totalWidth) * 0.5f;
    
    ImGui::SetCursorPosX(startX);
    
    for (size_t i = 0; i < contents.size(); ++i)
    {
        const auto& content = contents[i];
        bool isActive = activeContent && activeContent->IsSameContent(content.get());
        
        if (ImGui::Button(content->GetTitle(), ImVec2(buttonWidth, 30.0f)))
        {
            activeContent = content.get();
        }
        
        if (i < contents.size() - 1)
        {
            ImGui::SameLine(0, spacing);
        }
    }
}
}
} 