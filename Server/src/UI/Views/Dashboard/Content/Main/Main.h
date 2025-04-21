#pragma once

#include "../Content.h"
namespace UI {
namespace Views {

    class Main : public DashboardContent {
    public:
        Main(Application* app);
        ~Main() = default;

        void Render(int selectedPid = -1) override;
        const char* GetTitle() const override { return "Main"; }
        const char* GetContentId() const override { return "main"; }

    private:
        void RenderControls(int selectedPid);
    };
}
}