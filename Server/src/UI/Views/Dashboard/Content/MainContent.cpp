#include "MainContent.h"
#include <imgui.h>

namespace UI {
namespace Views {

DashboardMainContent::DashboardMainContent(Application* app)
{
    // Content'leri oluştur ve listeye ekle
    m_Contents.push_back(std::make_unique<FarmBot>(app));
    //m_Contents.push_back(std::make_unique<SaveLoad>(app));
}

void DashboardMainContent::Render(const DashboardContent* activeContent, int selectedPid)
{
    if (!activeContent)
    {
        // Default view veya hoş geldin mesajı
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - 100.0f, ImGui::GetWindowHeight() * 0.5f - 10.0f));
        ImGui::Text("Select a form from the header menu");
        return;
    }

    // Aktif content'i bul ve render et
    for (const auto& content : m_Contents)
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