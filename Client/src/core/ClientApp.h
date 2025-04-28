#pragma once

#include "PacketTypes.h"
#include "common/TaskManager.hpp"
#include "hack/globals/Globals.h"
#include "network/Network.h"
#include <Psapi.h>
#include <memory>
#include <string>
#include <windows.h>

namespace FracqClient {

constexpr int MEMORY_UPDATE_INTERVAL_MS = 250;
constexpr int SERVER_SYNC_INTERVAL_MS = 500;

constexpr int FARMBOT_INTERVAL_MS = 300;
constexpr int MAIN_INTERVAL_MS = 20;

class ClientApp {
public:
    ClientApp();
    ~ClientApp() = default;

    bool Initialize();

    bool RegisterWithServer();

    bool SendHealthCheck();

    bool IsRegistered() const;

    void ResetRegistration();

    const Packets::SettingsState& GetSettings() const { return m_SettingsState; }
    const Packets::MemoryState& GetMemoryState() const { return m_MemoryState; }
    bool ProcessSettingsPacket(const Packets::SettingsResponsePacket& packet);
    void UpdateMemoryState();
    Packets::ItemDumpState GetItemDumpState(const std::string& filter);

    bool RequestSettings();

    std::string GetLastError() const;

    Common::TaskManager* GetTaskManager() { return m_TaskManager.get(); }

private:
    std::unique_ptr<Network::NetworkClient> m_NetworkClient;

    std::unique_ptr<Common::TaskManager> m_TaskManager;

    bool m_Registered;
    std::string m_ServerIP;
    uint16_t m_ServerPort;
    std::string m_LastError;

    Packets::SettingsState m_SettingsState;
    Packets::MemoryState m_MemoryState;
    bool m_SettingsUpdated;

    bool CreateRegisterPacket(Packets::RegisterPacket& packet);
    void SetLastError(const std::string& error);
};

} // namespace FracqClient