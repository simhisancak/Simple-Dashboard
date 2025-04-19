#pragma once

#include "../Content.h"

namespace UI {
namespace Views {

    class FarmBot : public DashboardContent {
    public:
        FarmBot(Application* app);
        ~FarmBot() = default;

        void Render(int selectedPid = -1) override;
        const char* GetTitle() const override { return "FarmBot"; }
        const char* GetContentId() const override { return "farmbot"; }

    private:
        void RenderControls(int selectedPid);
        void RenderStatus(int selectedPid);
    };
}
}