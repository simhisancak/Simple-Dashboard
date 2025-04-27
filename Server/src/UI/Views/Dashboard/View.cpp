#include "View.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "UI/Theme.h"

namespace FracqServer {
namespace UI {
    namespace Views {

        DashboardView::DashboardView(Application* app, Renderer* renderer)
            : View(app, renderer)
            , m_Header(std::make_unique<DashboardHeader>())
            , m_Sidebar(std::make_unique<DashboardSidebar>(app))
            , m_MainContent(std::make_unique<DashboardMainContent>(app)) { }

        void DashboardView::Update() {
            if (m_SelectedPid != -1) {
                auto* serverManager = m_App->GetServerManager();
                auto* client = serverManager->FindClientByPID(m_SelectedPid);

                if (!client || !client->isActive) {
                    m_SelectedPid = -1;
                    m_ActiveContent = nullptr;
                }
            }
        }

        const char* DashboardView::GetViewId() const { return GetStaticViewId(); }

        std::unique_ptr<IViewState> DashboardView::CreateState() {
            return std::make_unique<DashboardViewState>();
        }

        void DashboardView::Render() {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar
                                            | ImGuiWindowFlags_NoCollapse
                                            | ImGuiWindowFlags_NoResize
                                            | ImGuiWindowFlags_NoBringToFrontOnFocus
                                            | ImGuiWindowFlags_NoNavFocus;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, ThemeConfig::WindowRounding);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, ThemeConfig::WindowBorderSize);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            if (ImGui::Begin("DashboardContainer", nullptr, window_flags)) {
                ImGui::PopStyleVar(3);

                RenderTitleBar("Dashboard", true);

                if (ImGui::BeginChild("Content", ImVec2(0, 0), false)) {

                    ImGui::BeginChild("SidebarContainer",
                                      ImVec2(ThemeConfig::SidebarWidth, 0),
                                      false,
                                      ImGuiWindowFlags_NoScrollbar
                                          | ImGuiWindowFlags_NoScrollWithMouse);
                    m_Sidebar->Render(m_SelectedPid);
                    ImGui::EndChild();

                    ImGui::SameLine(0, ThemeConfig::SeparatorPadding);

                    ThemeConfig::DrawSeparator(true, 0.98f);

                    ImGui::SameLine(0, ThemeConfig::SeparatorPadding);

                    ImGui::BeginChild("MainArea", ImVec2(0, 0), false);

                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ThemeConfig::BgMid());
                    ImGui::BeginChild("Header",
                                      ImVec2(ImGui::GetContentRegionAvail().x,
                                             ThemeConfig::HeaderHeight),
                                      false,
                                      ImGuiWindowFlags_NoScrollbar);
                    {
                        m_Header->Render(m_MainContent->GetContents(), m_ActiveContent);

                        ImGui::SetCursorPosY(ThemeConfig::HeaderHeight
                                             - ThemeConfig::SeparatorPadding);
                        ThemeConfig::DrawSeparator(false, 10.0f);
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor();

                    ImGui::BeginChild("MainContent",
                                      ImVec2(ImGui::GetContentRegionAvail().x, 0),
                                      false);
                    m_MainContent->Render(m_ActiveContent, m_SelectedPid);
                    ImGui::EndChild();

                    ImGui::EndChild();
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }
    } // namespace Views
} // namespace UI
} // namespace FracqServer
