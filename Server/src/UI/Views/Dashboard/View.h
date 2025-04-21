#pragma once

#include "Content/Content.h"
#include "Content/MainContent.h"
#include "Header.h"
#include "Sidebar.h"
#include "UI/View.h"
#include <memory>

namespace UI {
namespace Views {

    enum class DashboardContentType { None, Main, FarmBot, SaveLoad };

    class DashboardViewState : public IViewState {
    public:
        void Reset() override {
            activeContent = nullptr;
            selectedPid = -1;
        }

        const DashboardContent* activeContent = nullptr;
        int selectedPid = -1;
    };

    class DashboardView : public View {
    public:
        DashboardView(Application* app, Renderer* renderer);
        virtual ~DashboardView() = default;

        virtual void Update() override;
        virtual void Render() override;
        virtual const char* GetViewId() const override;
        virtual std::unique_ptr<IViewState> CreateState() override;

        static const char* GetStaticViewId() { return "DashboardView"; }

    private:
        void RenderDockspace();

        std::unique_ptr<DashboardHeader> m_Header;
        std::unique_ptr<DashboardSidebar> m_Sidebar;
        std::unique_ptr<DashboardMainContent> m_MainContent;

        const DashboardContent* m_ActiveContent = nullptr;
        int m_SelectedPid = -1;
    };
}
}