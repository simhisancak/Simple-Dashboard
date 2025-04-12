#include "ModalForm.h"
#include "imgui.h"

namespace UI::Components {

ModalForm::ModalForm(const std::string& title, float width, float height)
    : m_Title(title)
    , m_IsVisible(false)
    , m_Width(width)
    , m_Height(height)
    , m_Padding(16.0f)
    , m_Spacing(8.0f)
    , m_IsContentRendering(false)
{
}

void ModalForm::SetSize(float width, float height)
{
    m_Width = width;
    m_Height = height;
}

void ModalForm::Render()
{
    if (!m_IsVisible)
        return;

    // Form popup'ı aç
    ImGui::OpenPopup(m_Title.c_str());
    
    // Center the popup on the screen only when first appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    // Set the window size consistently
    ImGui::SetNextWindowSize(ImVec2(m_Width, m_Height), ImGuiCond_Always);

    // Apply more modern styling to the modal
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Padding, m_Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, m_Spacing));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    
    // Colors for a more attractive appearance
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.10f, 0.20f, 0.40f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.15f, 0.30f, 0.50f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

    // Allow dragging but prevent resizing
    ImGuiWindowFlags modalFlags = ImGuiWindowFlags_NoSavedSettings | 
                                 ImGuiWindowFlags_NoCollapse | 
                                 ImGuiWindowFlags_NoResize;

    // Modal popup başlat
    if (ImGui::BeginPopupModal(m_Title.c_str(), &m_IsVisible, modalFlags))
    {
        // Form içeriği
        if (m_ContentRenderer) {
            // Use custom content renderer if provided
            m_ContentRenderer();
        }
        else if (m_Layout) {
            // Otherwise fall back to layout-based rendering
            const float contentAvailHeight = m_Height - ImGui::GetCursorPosY() - m_Padding * 2;
            ImGui::BeginChild("ModalFormContent", ImVec2(0, contentAvailHeight), false);
            
            m_Layout->SetPosition(0, ImGui::GetCursorPosY());
            m_Layout->SetSpacing(m_Spacing);
            m_Layout->SetPadding(m_Padding);
            m_Layout->Render();
            
            ImGui::EndChild();
        }

        // ESC tuşu kontrolü
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            m_IsVisible = false;
        }

        ImGui::EndPopup();
    }

    // Restore default styles
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(4);
}

bool ModalForm::BeginContent()
{
    if (!m_IsVisible)
        return false;
        
    // Form stili
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Padding, m_Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, m_Spacing));
    
    // İçerik alanını başlat
    m_IsContentRendering = true;
    return true;
}

void ModalForm::EndContent()
{
    if (m_IsContentRendering) {
        ImGui::PopStyleVar(2);
        m_IsContentRendering = false;
    }
}
} 