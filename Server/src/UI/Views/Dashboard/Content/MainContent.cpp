#include "MainContent.h"
#include <imgui.h>

namespace UI
{
    namespace Views
    {

        DashboardMainContent::DashboardMainContent(Application *app)
        {
            m_Contents.push_back(std::make_unique<FarmBot>(app));
        }

        void DashboardMainContent::Render(const DashboardContent *activeContent, int selectedPid)
        {
            if (!activeContent)
            {
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - 100.0f, ImGui::GetWindowHeight() * 0.5f - 10.0f));
                ImGui::Text("Select a form from the header menu");
                return;
            }

            for (const auto &content : m_Contents)
            {
                if (content->IsSameContent(activeContent))
                {
                    content->Render(selectedPid);
                    break;
                }
            }
        }
    }
}