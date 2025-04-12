#pragma once

#include <windows.h>
#include <string>
#include <memory>
#include <Psapi.h>
#include "network/Network.h"
#include "common/TaskManager.hpp"
#include "PacketTypes.h"
#include "hack/globals/Globals.h"

// Constants for task scheduling intervals (in milliseconds)
constexpr int MEMORY_UPDATE_INTERVAL_MS = 250;
constexpr int SERVER_SYNC_INTERVAL_MS = 500;

constexpr int FARMBOT_INTERVAL_MS = 100;


class ClientApp {
public:
    ClientApp();
    ~ClientApp();

    // Initialize client app
    bool Initialize();

    // Register with server
    bool RegisterWithServer();
    
    // Send health check to server and get response
    bool SendHealthCheck();
    
    // Check if client is registered with server
    bool IsRegistered() const;
    
    // Reset registration status to force reconnect
    void ResetRegistration();
    
    // State management
    const Packets::SettingsState& GetSettings() const { return m_SettingsState; }
    const Packets::MemoryState& GetMemoryState() const { return m_MemoryState; }
    bool ProcessSettingsPacket(const Packets::SettingsResponsePacket& packet);
    void UpdateMemoryState();
    void UpdateMemoryState(const Packets::MemoryState& newState);
    
    // Combined settings and commands handling
    bool RequestSettings();

    // Status and error handling
    std::string GetLastError() const;

    // Task Manager access
    Common::TaskManager* GetTaskManager() { return m_TaskManager.get(); }

private:
    // Network client
    std::unique_ptr<Network::NetworkClient> m_NetworkClient;
    
    // Task Manager for handling threads
    std::unique_ptr<Common::TaskManager> m_TaskManager;
    
    // State
    bool m_Registered;
    std::string m_ServerIP;
    uint16_t m_ServerPort;
    std::string m_LastError;
    
    // State
    Packets::SettingsState m_SettingsState;
    Packets::MemoryState m_MemoryState;
    bool m_SettingsUpdated;
    
    // Helpers
    bool CreateRegisterPacket(Packets::RegisterPacket& packet);
    void SetLastError(const std::string& error);
};