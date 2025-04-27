#pragma once

#include "../Content.h"
#include "Item.h"

namespace FracqServer {
namespace UI {
    namespace Views {

        class Pickup : public DashboardContent {
        public:
            Pickup(Application* app);
            ~Pickup() = default;

            void Render(int selectedPid = -1) override;
            const char* GetTitle() const override { return "Pickup"; }
            const char* GetContentId() const override { return "pickup"; }

        private:
            void RenderControls(int selectedPid);
            char m_FilterBuffer[Packets::MAX_NAME_LENGTH] = "";
        };
    } // namespace Views
} // namespace UI
} // namespace FracqServer
