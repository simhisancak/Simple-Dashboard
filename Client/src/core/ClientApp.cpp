#include "ClientApp.h"

#include <algorithm>
#include <iostream>

#include "Memory.h"
#include "common/Logger.h"
#include "../common/Helper.h"
#include "features/Features.h"
#include "features/farmbot/FarmBot.h"
#include "features/main/Main.h"

namespace FracqClient {

ClientApp::ClientApp()
    : m_NetworkClient(std::make_unique<Network::NetworkClient>(this))
    , m_TaskManager(std::make_unique<Common::TaskManager>())
    , m_Registered(false)
    , m_ServerIP("127.0.0.1")
    , m_ServerPort(8888)
    , m_SettingsUpdated(false) {
    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Creating client application");
    LOG_INFO(LOG_COMPONENT_CLIENTAPP,
             "Server configuration - " << m_ServerIP << ":" << m_ServerPort);

    ZeroMemory(&m_SettingsState, sizeof(m_SettingsState));
}

bool ClientApp::Initialize() {
    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Initializing client");
    if (!m_NetworkClient->Initialize()) {
        return false;
    }

    MODULEINFO moduleInfo = { 0 };
    HMODULE hModule = GetModuleHandle(NULL);

    if (hModule
        && GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
        m_MemoryState.ModuleBase = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
        m_MemoryState.ModuleSize = moduleInfo.SizeOfImage;
    } else {
        LOG_ERROR(LOG_COMPONENT_CLIENTAPP, "Failed to get module information");
    }

    m_MemoryState.hproc = GetCurrentProcess();
    Globals::Get()->Initialize();
    Features::Initialize<FarmBot>(this, FARMBOT_INTERVAL_MS);
    Features::Initialize<Main>(this, MAIN_INTERVAL_MS);

    m_TaskManager->ScheduleRecurringTask([this]() { UpdateMemoryState(); },
                                         std::chrono::milliseconds(MEMORY_UPDATE_INTERVAL_MS));

    m_TaskManager->ScheduleRecurringTask(
        [this]() {
            if (IsRegistered()) {
                RequestSettings();
            }
        },
        std::chrono::milliseconds(SERVER_SYNC_INTERVAL_MS));

    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Client initialized successfully");
    return true;
}

void ClientApp::UpdateMemoryState() {
    uint64_t oldModuleBase = m_MemoryState.ModuleBase;
    uint32_t oldModuleSize = m_MemoryState.ModuleSize;
    HANDLE oldHandle = m_MemoryState.hproc;

    m_MemoryState = Packets::MemoryState();

    m_MemoryState.ModuleBase = oldModuleBase;
    m_MemoryState.ModuleSize = oldModuleSize;
    m_MemoryState.hproc = oldHandle;

    auto mobs = InstanceHelper::getMobs(MobType::All);
    m_MemoryState.MobListSize = std::min<size_t>(mobs.size(), Packets::MAX_MOB_LIST_SIZE);

    for (size_t i = 0; i < m_MemoryState.MobListSize; i++) {
        m_MemoryState.MobList[i] = mobs[i];
    }

    auto mainActor = InstanceHelper::GetMainActor();
    Packets::Instance instance;
    instance.VID = mainActor.GetVID();
    strcpy(instance.Name, mainActor.GetName().c_str());
    instance.Position = mainActor.GetPixelPosition();
    instance.Type = mainActor.GetType();
    m_MemoryState.MainActor = instance;
}

Packets::ItemDumpState ClientApp::GetItemDumpState(const std::string& filter) {
    Packets::ItemDumpState itemDumpState;
    itemDumpState.ItemListSize = 0;

    auto itemDumps = ItemHelper::getItemList();
    size_t addedItems = 0;

    for (const auto& item : itemDumps) {
        std::string itemName = item.GetName();

        if (filter.empty() || Common::Helper::ContainsCaseInsensitive(itemName, filter)) {
            if (addedItems < Packets::MAX_ITEM_LIST_SIZE) {
                itemDumpState.ItemList[addedItems].Vnum = item.GetVnum();
                strcpy(itemDumpState.ItemList[addedItems].Name, itemName.c_str());
                addedItems++;
            }
        }
    }

    itemDumpState.ItemListSize = addedItems;
    return itemDumpState;
}

bool ClientApp::RegisterWithServer() {
    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Registering with server");
    ResetRegistration();
    LOG_DEBUG(LOG_COMPONENT_CLIENTAPP,
              "Connecting to server at " << m_ServerIP << ":" << m_ServerPort);

    if (!m_NetworkClient->Connect(m_ServerIP, m_ServerPort)) {
        return false;
    }

    Packets::RegisterPacket packet;
    if (!CreateRegisterPacket(packet)) {
        LOG_ERROR(LOG_COMPONENT_CLIENTAPP, "Failed to create register packet");
        m_NetworkClient->Disconnect();
        return false;
    }

    LOG_DEBUG(LOG_COMPONENT_CLIENTAPP, "Sending registration packet");
    if (!m_NetworkClient->SendRegisterPacket(packet)) {
        m_NetworkClient->Disconnect();
        return false;
    }

    m_Registered = true;
    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Successfully registered with server");
    return true;
}

bool ClientApp::SendHealthCheck() {
    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Sending health check to server");

    if (!m_NetworkClient || !m_NetworkClient->IsConnected()) {
        SetLastError("Client not connected to server");
        LOG_ERROR(LOG_COMPONENT_CLIENTAPP, m_LastError);
        return false;
    }

    if (!m_Registered) {
        SetLastError("Client not registered with server");
        LOG_ERROR(LOG_COMPONENT_CLIENTAPP, m_LastError);
        return false;
    }

    if (!m_NetworkClient->SendHealthCheckPacket()) {
        m_Registered = false;
        return false;
    }

    return true;
}

bool ClientApp::IsRegistered() const { return m_Registered; }

void ClientApp::ResetRegistration() {
    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Resetting registration status");
    m_Registered = false;

    if (m_NetworkClient) {
        m_NetworkClient->Disconnect();
    }

    m_LastError = "";
}

bool ClientApp::ProcessSettingsPacket(const Packets::SettingsResponsePacket& packet) {
    m_SettingsState = packet.State;
    return true;
}

std::string ClientApp::GetLastError() const { return m_LastError; }

void ClientApp::SetLastError(const std::string& error) { m_LastError = error; }

bool ClientApp::CreateRegisterPacket(Packets::RegisterPacket& packet) {
    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Creating register packet");

    packet.Header = Packets::PacketType::Register;
    packet.PID = GetCurrentProcessId();
    LOG_DEBUG(LOG_COMPONENT_CLIENTAPP, "Using PID: " << packet.PID);

    char moduleName[MAX_PATH];
    if (GetModuleFileNameA(NULL, moduleName, MAX_PATH) == 0) {
        SetLastError("Failed to get module filename: " + std::to_string(::GetLastError()));
        LOG_ERROR(LOG_COMPONENT_CLIENTAPP, m_LastError);
        return false;
    }

    char* fileName = strrchr(moduleName, '\\');
    if (fileName) {
        fileName++;
    } else {
        fileName = moduleName;
    }

    strncpy_s(packet.ClientName, fileName, sizeof(packet.ClientName) - 1);
    LOG_DEBUG(LOG_COMPONENT_CLIENTAPP, "Using client name: " << packet.ClientName);

    return true;
}

bool ClientApp::RequestSettings() {
    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Requesting settings from server");

    if (!m_NetworkClient || !m_NetworkClient->IsConnected()) {
        SetLastError("Client not connected to server");
        LOG_ERROR(LOG_COMPONENT_CLIENTAPP, m_LastError);
        return false;
    }

    Packets::SettingsRequestPacket packet;
    if (!m_NetworkClient->SendSettingsRequestPacket(packet)) {
        return false;
    }

    return true;
}

} // namespace FracqClient