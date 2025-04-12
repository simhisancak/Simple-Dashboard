#include "View.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace UI {
namespace Views {

DashboardView::DashboardView(Application* app, Renderer* renderer)
    : View(app, renderer)
    , m_Header(std::make_unique<DashboardHeader>())
    , m_Sidebar(std::make_unique<DashboardSidebar>(app))
    , m_MainContent(std::make_unique<DashboardMainContent>(app))
{
}

void DashboardView::Update()
{
    if (m_SelectedPid != -1)
    {
        auto* serverManager = m_App->GetServerManager();
        auto* client = serverManager->FindClientByPID(m_SelectedPid);
        
        if (!client || !client->isActive)
        {
            m_SelectedPid = -1;
            m_ActiveContent = nullptr;
        }
    }
}

const char* DashboardView::GetViewId() const
{
    return GetStaticViewId();
}

std::unique_ptr<IViewState> DashboardView::CreateState()
{
    return std::make_unique<DashboardViewState>();
}

void DashboardView::Render()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                  ImGuiWindowFlags_NoCollapse | 
                                  ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoBringToFrontOnFocus | 
                                  ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("DashboardContainer", nullptr, window_flags))
    {
        ImGui::PopStyleVar(3);

        RenderTitleBar("Dashboard", true);

        if (ImGui::BeginChild("Content", ImVec2(0, 0), false))
        {
            ImGui::BeginChild("Sidebar", ImVec2(200.0f, 0), false);
            m_Sidebar->Render(m_SelectedPid);
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
            ImGui::SameLine();

            ImGui::BeginGroup();
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
                ImGui::BeginChild("Header", ImVec2(ImGui::GetContentRegionAvail().x - 10.0f, 50.0f), true, ImGuiWindowFlags_NoScrollbar);
                {
                    const ImVec2 windowPos = ImGui::GetWindowPos();
                    const ImVec2 windowSize = ImGui::GetWindowSize();
                    auto* drawList = ImGui::GetWindowDrawList();
                    
                    drawList->AddRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), 
                                    ImGui::GetColorU32(ImGuiCol_Border), 0.0f, 0, 0.0f);
                    
                    drawList->AddLine(ImVec2(windowPos.x, windowPos.y + windowSize.y),
                                    ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
                                    ImGui::GetColorU32(ImGuiCol_Border));

                    m_Header->Render(m_MainContent->GetContents(), m_ActiveContent);
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();

                ImGui::BeginChild("MainContent", ImVec2(0, 0), false);
                m_MainContent->Render(m_ActiveContent, m_SelectedPid);
                ImGui::EndChild();
            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
}
} 