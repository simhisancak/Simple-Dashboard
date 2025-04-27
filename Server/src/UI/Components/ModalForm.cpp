#include "ModalForm.h"
#include "imgui.h"

namespace FracqServer {
namespace UI {
    namespace Components {

        ModalForm::ModalForm(const std::string& title, float width, float height)
            : m_Title(title)
            , m_IsVisible(false)
            , m_Width(width)
            , m_Height(height)
            , m_Padding(16.0f)
            , m_Spacing(8.0f)
            , m_IsContentRendering(false) { }

        void ModalForm::SetSize(float width, float height) {
            m_Width = width;
            m_Height = height;
        }

        void ModalForm::Render() {
            if (!m_IsVisible)
                return;

            ImGui::OpenPopup(m_Title.c_str());

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            ImGui::SetNextWindowSize(ImVec2(m_Width, m_Height), ImGuiCond_Always);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Padding, m_Padding));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, m_Spacing));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

            ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.10f, 0.20f, 0.40f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.15f, 0.30f, 0.50f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

            ImGuiWindowFlags modalFlags = ImGuiWindowFlags_NoSavedSettings
                                          | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

            if (ImGui::BeginPopupModal(m_Title.c_str(), &m_IsVisible, modalFlags)) {
                if (m_ContentRenderer) {
                    m_ContentRenderer();
                } else if (m_Layout) {
                    const float contentAvailHeight = m_Height - ImGui::GetCursorPosY()
                                                     - m_Padding * 2;
                    ImGui::BeginChild("ModalFormContent", ImVec2(0, contentAvailHeight), false);

                    m_Layout->SetPosition(0, ImGui::GetCursorPosY());
                    m_Layout->SetSpacing(m_Spacing);
                    m_Layout->SetPadding(m_Padding);
                    m_Layout->Render();

                    ImGui::EndChild();
                }

                if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    m_IsVisible = false;
                }

                ImGui::EndPopup();
            }

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(4);
        }

        bool ModalForm::BeginContent() {
            if (!m_IsVisible)
                return false;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Padding, m_Padding));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, m_Spacing));

            m_IsContentRendering = true;
            return true;
        }

        void ModalForm::EndContent() {
            if (m_IsContentRendering) {
                ImGui::PopStyleVar(2);
                m_IsContentRendering = false;
            }
        }
    } // namespace Components
} // namespace UI
} // namespace FracqServer
