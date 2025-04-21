#include "View.h"

#include <Windows.h>
#include <imgui.h>

#include "Theme.h"
#include "Utils/Injector/Injector.h"

namespace UI {
namespace Views {

    View::View(Application* app, Renderer* renderer)
        : m_App(app)
        , m_Renderer(renderer) { }

    bool View::TryInject(const char* processName, DWORD pid) {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string exePath(buffer);
        std::string exeDir = exePath.substr(0, exePath.find_last_of("\\/"));
        std::string dllPath = exeDir + "\\FracqClient.dll";

        Injector injector;
        bool success = false;

        if (processName && strlen(processName) > 0) {
            success = injector.InjectByProcessName(processName, dllPath);
        } else if (pid > 0) {
            success = injector.InjectByPID(pid, dllPath);
        }

        if (success) {
            m_ShowInjectModal = false;
            return true;
        } else {
            m_LastError = injector.GetLastError();
            return false;
        }
    }

    void View::RenderTitleBar(const char* title, bool showExitButton) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ThemeConfig::BgMid());
        ImGui::BeginChild("##titlebar",
                          ImVec2(ImGui::GetWindowWidth(), ThemeConfig::TitleBarHeight),
                          false,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        // Title text - left aligned with padding
        ImGui::SetCursorPosY((ThemeConfig::TitleBarHeight - ImGui::GetTextLineHeight()) * 0.5f);
        ImGui::SetCursorPosX(ThemeConfig::ViewPadding);
        ImGui::Text(title);

        if (showExitButton) {
            float windowWidth = ImGui::GetWindowWidth();
            float buttonSize = ThemeConfig::TitleBarHeight - 8.0f; // Square button size
            float buttonY = (ThemeConfig::TitleBarHeight - buttonSize) * 0.5f; // Center vertically

            // Close button
            float closeButtonX = windowWidth - buttonSize - 4.0f; // 4px padding from right
            ImGui::SetCursorPos(ImVec2(closeButtonX, buttonY));

            // Button styling for close button
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.6f, 0.0f, 0.0f, 0.0f)); // Transparent red
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(0.8f, 0.0f, 0.0f, 1.0f)); // Bright red on hover
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                  ImVec4(0.9f, 0.0f, 0.0f, 1.0f)); // Brighter red on click

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            if (ImGui::Button("×", ImVec2(buttonSize, buttonSize))) {
                PostQuitMessage(0);
            }
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);

            // Minimize button
            float minimizeButtonX = closeButtonX - buttonSize - 4.0f; // 4px spacing between buttons
            ImGui::SetCursorPos(ImVec2(minimizeButtonX, buttonY));

            // Button styling for minimize button
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.2f, 0.2f, 0.2f, 0.0f)); // Transparent gray
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(0.3f, 0.3f, 0.3f, 1.0f)); // Gray on hover
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                  ImVec4(0.4f, 0.4f, 0.4f, 1.0f)); // Darker gray on click

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            if (ImGui::Button("_", ImVec2(buttonSize, buttonSize))) {
                ShowWindow(GetActiveWindow(), SW_MINIMIZE);
            }
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);

            // Inject button
            float injectButtonX = minimizeButtonX - buttonSize
                                  - 4.0f; // 4px spacing between buttons
            ImGui::SetCursorPos(ImVec2(injectButtonX, buttonY));

            // Button styling for inject button
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.0f, 0.2f, 0.0f, 0.0f)); // Transparent gray
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(.0f, 0.3f, .0f, 1.0f)); // Gray on hover
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                  ImVec4(.0f, 0.4f, .0f, 1.0f)); // Darker gray on click

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            if (ImGui::Button("+", ImVec2(buttonSize, buttonSize))) {
                m_ShowInjectModal = true;
            }
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
            if (m_ShowInjectModal) {
                RenderInjectModal();
            }
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
            HWND hwnd = m_Renderer->GetWindow()->GetHandle();
            SetCapture(hwnd);
            POINT cursorPos;
            GetCursorPos(&cursorPos);

            RECT windowRect;
            GetWindowRect(hwnd, &windowRect);

            m_LastMouseX = static_cast<float>(cursorPos.x - windowRect.left);
            m_LastMouseY = static_cast<float>(cursorPos.y - windowRect.top);
            m_IsDragging = true;
        } else if (m_IsDragging && ImGui::IsMouseDragging(0)) {
            HWND hwnd = m_Renderer->GetWindow()->GetHandle();
            POINT cursorPos;
            GetCursorPos(&cursorPos);

            int newX = cursorPos.x - static_cast<int>(m_LastMouseX);
            int newY = cursorPos.y - static_cast<int>(m_LastMouseY);

            SetWindowPos(hwnd, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        } else if (ImGui::IsMouseReleased(0)) {
            m_IsDragging = false;
            ReleaseCapture();
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    void View::RenderInjectModal() {
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);

        ImGui::OpenPopup("Inject Process");

        static bool useProcessName = true;
        static char processNameBuffer[256] = "";
        static char pidBuffer[32] = "";
        static bool hasError = false;

        if (ImGui::BeginPopupModal("Inject Process",
                                   &m_ShowInjectModal,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::RadioButton("Process Name", useProcessName))
                useProcessName = true;
            ImGui::SameLine();
            if (ImGui::RadioButton("Process ID", !useProcessName))
                useProcessName = false;

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                                ImVec2(0, ThemeConfig::SeparatorPadding));
            ImGui::Separator();
            ImGui::PopStyleVar();

            bool shouldInject = false;
            if (useProcessName) {
                ImGui::Text("Enter Process Name:");
                shouldInject = ImGui::InputText("##processname",
                                                processNameBuffer,
                                                sizeof(processNameBuffer),
                                                ImGuiInputTextFlags_EnterReturnsTrue);
            } else {
                ImGui::Text("Enter Process ID (PID):");
                shouldInject = ImGui::InputText("##pid",
                                                pidBuffer,
                                                sizeof(pidBuffer),
                                                ImGuiInputTextFlags_CharsDecimal
                                                    | ImGuiInputTextFlags_EnterReturnsTrue);
            }

            // Hata mesajını göster
            if (hasError && !m_LastError.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                ImGui::TextWrapped("Injection failed: %s", m_LastError.c_str());
                ImGui::PopStyleColor();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                                ImVec2(0, ThemeConfig::SeparatorPadding));
            ImGui::Separator();
            ImGui::PopStyleVar();

            float windowWidth = ImGui::GetWindowWidth();
            float buttonWidth = 120;
            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

            shouldInject |= ImGui::Button("Inject", ImVec2(buttonWidth, 0));

            if (shouldInject) {
                if (TryInject(useProcessName ? processNameBuffer : nullptr,
                              useProcessName ? 0 : atoi(pidBuffer))) {
                    memset(processNameBuffer, 0, sizeof(processNameBuffer));
                    memset(pidBuffer, 0, sizeof(pidBuffer));
                    hasError = false;
                    m_ShowInjectModal = false;
                } else {
                    hasError = true;
                }
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                hasError = false;
                m_LastError.clear();
                memset(processNameBuffer, 0, sizeof(processNameBuffer));
                memset(pidBuffer, 0, sizeof(pidBuffer));
                m_ShowInjectModal = false;
            }

            ImGui::EndPopup();
        } else {
            // Modal kapandığında hata mesajını ve input'ları temizle
            hasError = false;
            m_LastError.clear();
            memset(processNameBuffer, 0, sizeof(processNameBuffer));
            memset(pidBuffer, 0, sizeof(pidBuffer));
        }
    }

} // namespace Views
} // namespace UI
