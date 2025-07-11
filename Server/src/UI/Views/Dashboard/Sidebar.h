#pragma once
#include "Application.h"
#include "PacketTypes.h"
#include "Server/ServerManager.h"
#include <string>

namespace FracqServer {
namespace UI {
    namespace Views {

        class DashboardSidebar {
        public:
            DashboardSidebar(Application* app);
            ~DashboardSidebar() = default;

            void Render(int& selectedPid);

        private:
            Application* m_App;
        };
    } // namespace Views
} // namespace UI
} // namespace FracqServer
