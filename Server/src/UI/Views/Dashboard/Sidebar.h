#pragma once
#include "Application.h"
#include "Server/ServerManager.h"
#include "PacketTypes.h"
#include <string>

namespace UI {
namespace Views {

class DashboardSidebar {
public:
    DashboardSidebar(Application* app);
    ~DashboardSidebar() = default;

    void Render(int& selectedPid);

private:
    void RenderInjectModal();
    bool TryInject(const char* processName, DWORD pid);

    Application* m_App;
    bool m_ShowInjectModal = false;
    char m_PidInputBuffer[16] = "";
    std::string m_LastError;
};
}
} 