#include "LoginView.h"
#include "Auth/AuthManager.h"
#include "imgui.h"

namespace FracqServer {
namespace UI {
    namespace Views {
        void LoginView::RenderLoginForm() {
            const float contentWidth = 300.0f;
            const float windowWidth = ImGui::GetWindowWidth();
            const float windowHeight = ImGui::GetWindowHeight();
            const float startX = (windowWidth - contentWidth) * 0.5f;
            const float startY = windowHeight * 0.3f;

            ImGui::SetCursorPos(ImVec2(startX, startY));
            ImGui::BeginChild("LoginFormContainer", ImVec2(contentWidth, 0), false);

            const std::string& error = m_App->GetAuthManager()->GetLastError();
            if (!error.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
                ImGui::TextWrapped("%s", error.c_str());
                ImGui::PopStyleColor();
                ImGui::Spacing();
            }

            m_UsernameInput->SetSize(contentWidth - 16, 30);
            m_PasswordInput->SetSize(contentWidth - 16, 30);
            m_ShowPasswordCheckbox->SetSize(contentWidth - 16, 20);
            m_LoginButton->SetSize(contentWidth - 16, 35);

            m_FormLayout->SetPosition(8, ImGui::GetCursorPosY());
            m_FormLayout->SetSpacing(10.0f);
            m_FormLayout->SetPadding(0.0f);
            m_FormLayout->Render();

            ImGui::EndChild();
        }
    } // namespace Views
} // namespace UI
} // namespace FracqServer
