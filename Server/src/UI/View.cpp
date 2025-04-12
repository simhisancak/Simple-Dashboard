#include "View.h"
#include <Windows.h>
#include <imgui.h>

namespace UI {
namespace Views {

View::View(Application* app, Renderer* renderer)
    : m_App(app), m_Renderer(renderer)
{
}

void View::RenderTitleBar(const char* title, bool showExitButton)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::BeginChild("##titlebar", ImVec2(ImGui::GetWindowWidth(), 32.0f), false);

    ImGui::SetCursorPosY(8.0f);
    ImGui::SetCursorPosX(10.0f);
    ImGui::Text(title);

    if (showExitButton)
    {
        float buttonHeight = 22.0f;
        float buttonY = (32.0f - buttonHeight) * 0.5f;
        float windowWidth = ImGui::GetWindowWidth();
        float buttonX = windowWidth - 40.0f;
        
        ImGui::SetCursorPos(ImVec2(buttonX, buttonY));
        if (ImGui::Button("X", ImVec2(30.0f, buttonHeight)))
        {
            PostQuitMessage(0);
        }
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
    {
        HWND hwnd = m_Renderer->GetWindow()->GetHandle();
        SetCapture(hwnd);
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        
        RECT windowRect;
        GetWindowRect(hwnd, &windowRect);
        
        m_LastMouseX = static_cast<float>(cursorPos.x - windowRect.left);
        m_LastMouseY = static_cast<float>(cursorPos.y - windowRect.top);
        m_IsDragging = true;
    }
    else if (m_IsDragging && ImGui::IsMouseDragging(0))
    {
        HWND hwnd = m_Renderer->GetWindow()->GetHandle();
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        
        int newX = cursorPos.x - static_cast<int>(m_LastMouseX);
        int newY = cursorPos.y - static_cast<int>(m_LastMouseY);
        
        SetWindowPos(hwnd, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
    else if (ImGui::IsMouseReleased(0))
    {
        m_IsDragging = false;
        ReleaseCapture();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

} // namespace Views
} // namespace UI 