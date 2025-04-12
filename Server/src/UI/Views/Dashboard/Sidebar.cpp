#include "Sidebar.h"   
#include <imgui.h>
#include "Utils/Injector/Injector.h"
#include <windows.h>

namespace UI {
namespace Views {

DashboardSidebar::DashboardSidebar(Application* app)
    : m_App(app)
{
}

bool DashboardSidebar::TryInject(const char* processName, DWORD pid) {
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

void DashboardSidebar::Render(int& selectedPid)
{
    ImGui::BeginChild("Sidebar", ImVec2(200, 0), true);
    
    ImGui::Text("Connected Apps");
    ImGui::Separator();
    
    const auto& clients = m_App->GetServerManager()->GetConnectedClients();
    
    for (const auto& client : clients)
    {
        bool isSelected = (selectedPid == client->pid);
        std::string label = client->processName + " (PID: " + std::to_string(client->pid) + ")";
        
        if (ImGui::Selectable(label.c_str(), isSelected))
        {
            selectedPid = client->pid;
        }
        
        if (isSelected)
        {
            ImGui::SetItemDefaultFocus();
        }
    }

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 40);
    ImGui::Separator();
    
    float windowWidth = ImGui::GetWindowWidth();
    float buttonWidth = 30;
    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
    
    if (ImGui::Button("+", ImVec2(buttonWidth, 0))) {
        m_ShowInjectModal = true;
    }
    
    ImGui::EndChild();

    if (m_ShowInjectModal) {
        RenderInjectModal();
    }
}

void DashboardSidebar::RenderInjectModal()
{
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
    ImGui::OpenPopup("Inject Process");
    
    static bool useProcessName = true;
    static char processNameBuffer[256] = "";
    static char pidBuffer[32] = "";
    
    if (ImGui::BeginPopupModal("Inject Process", &m_ShowInjectModal, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::RadioButton("Process Name", useProcessName)) useProcessName = true;
        ImGui::SameLine();
        if (ImGui::RadioButton("Process ID", !useProcessName)) useProcessName = false;

        ImGui::Separator();

        bool shouldInject = false;
        if (useProcessName) {
            ImGui::Text("Enter Process Name:");
            shouldInject = ImGui::InputText("##processname", processNameBuffer, sizeof(processNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
        } else {
            ImGui::Text("Enter Process ID (PID):");
            shouldInject = ImGui::InputText("##pid", pidBuffer, sizeof(pidBuffer), ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue);
        }

        ImGui::Separator();
        
        float windowWidth = ImGui::GetWindowWidth();
        float buttonWidth = 120;
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
        
        shouldInject |= ImGui::Button("Inject", ImVec2(buttonWidth, 0));

        if (shouldInject) {
            if (TryInject(useProcessName ? processNameBuffer : nullptr, useProcessName ? 0 : atoi(pidBuffer))) {
                memset(processNameBuffer, 0, sizeof(processNameBuffer));
                memset(pidBuffer, 0, sizeof(pidBuffer));
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Injection failed: %s", m_LastError.c_str());
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            m_ShowInjectModal = false;
        }

        ImGui::EndPopup();
    }
}

}
} 