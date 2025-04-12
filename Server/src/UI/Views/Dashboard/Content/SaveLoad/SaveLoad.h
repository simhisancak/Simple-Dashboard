#pragma once

#include "../Content.h"

namespace UI {
namespace Views {

class SaveLoad : public DashboardContent {
public:
    SaveLoad(Application* app);
    ~SaveLoad() = default;

    void Render(int selectedPid = -1) override;
    const char* GetTitle() const override { return "Save/Load"; }
    const char* GetContentId() const override { return "saveload"; }

private:
    void RenderSaveSection();
    void RenderLoadSection();
};
}
} 