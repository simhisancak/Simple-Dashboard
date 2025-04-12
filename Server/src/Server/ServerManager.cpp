#include "ServerManager.h"
#include <winsock2.h>  // Must be included before windows.h
#include <ws2tcpip.h>
#include <windows.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <memory>
#include <fstream>
#include <algorithm>
#include "PacketTypes.h"
#include "Memory.h"  // For MAX_MOB_LIST_SIZE
#include <chrono>
#include <unordered_map>
#include <queue>
#include "../common/Logger.h"
#include "../Application.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace Server {

ServerManager::ServerManager(Application* app)
    : m_App(app)
    , m_Running(false)
    , m_ListenSocket(INVALID_SOCKET)
    , m_Port(0)
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        LOG_ERROR(LOG_COMPONENT_SERVER, "WSAStartup failed: " << std::to_string(result));
    }
}

ServerManager::~ServerManager()
{
    Stop();
    WSACleanup();
}

bool ServerManager::Start(int port)
{
    if (m_Running) {
        LOG_WARN(LOG_COMPONENT_SERVER, "Server is already running");
        return false;
    }

    m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_ListenSocket == INVALID_SOCKET) {
        LOG_ERROR(LOG_COMPONENT_SERVER, "Socket creation failed: " << std::to_string(WSAGetLastError()));
        return false;
    }

    u_long mode = 1;
    if (ioctlsocket(m_ListenSocket, FIONBIO, &mode) == SOCKET_ERROR) {
        LOG_ERROR(LOG_COMPONENT_SERVER, "Failed to set socket to non-blocking mode");
        closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
        return false;
    }

    int opt = 1;
    if (setsockopt(m_ListenSocket, SOL_SOCKET, SO_REUSEADDR, 
                   reinterpret_cast<char*>(&opt), sizeof(opt)) == SOCKET_ERROR) {
        LOG_ERROR(LOG_COMPONENT_SERVER, "setsockopt SO_REUSEADDR failed");
        closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
        return false;
    }

    sockaddr_in serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(static_cast<u_short>(port));

    if (bind(m_ListenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        LOG_ERROR(LOG_COMPONENT_SERVER, "Bind failed: " << std::to_string(WSAGetLastError()));
        closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
        return false;
    }

    if (listen(m_ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        LOG_ERROR(LOG_COMPONENT_SERVER, "Listen failed: " << std::to_string(WSAGetLastError()));
        closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
        return false;
    }

    m_Port = port;
    m_Running = true;

    m_ServerThread = std::thread(&ServerManager::ServerThread, this);
    m_AcceptThread = std::thread(&ServerManager::AcceptThread, this);
    
    LOG_INFO(LOG_COMPONENT_SERVER, "Server started successfully on port " << std::to_string(port));
    return true;
}

void ServerManager::ServerThread()
{
    DWORD lastMemoryRequestTime = GetTickCount();

    while (m_Running) {
        try {
            std::vector<std::shared_ptr<ClientInfo>> clientsToRemove;
            
            {
                std::shared_lock<std::shared_mutex> lock(m_ClientsMutex);
                for (const auto& client : m_Clients) {
                    if (client->IsTimedOut()) {
                        clientsToRemove.push_back(client);
                    }
                }
            }

            if (!clientsToRemove.empty()) {
                std::unique_lock<std::shared_mutex> lock(m_ClientsMutex);
                for (const auto& client : clientsToRemove) {
                    LOG_INFO(LOG_COMPONENT_SERVER, "Removing inactive client: " << client->processName 
                        << " (PID: " << client->pid << ")");
                    
                    if (client->socket != INVALID_SOCKET) {
                        shutdown(client->socket, SD_BOTH);
                        closesocket(client->socket);
                        client->socket = INVALID_SOCKET;
                    }
                    
                    m_Clients.erase(
                        std::remove_if(m_Clients.begin(), m_Clients.end(),
                            [&](const auto& c) { return c->pid == client->pid; }),
                        m_Clients.end());
                }
            }

            DWORD currentTime = GetTickCount();
            if (currentTime - lastMemoryRequestTime >= MEMORY_REQUEST_INTERVAL_MS) {
                std::shared_lock<std::shared_mutex> lock(m_ClientsMutex);
                for (const auto& client : m_Clients) {
                    if (client->isActive && client->socket != INVALID_SOCKET) {
                        RequestMemoryInfo(client->pid);
                    }
                }
                lastMemoryRequestTime = currentTime;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        catch (const std::exception& e) {
            LOG_ERROR(LOG_COMPONENT_SERVER, "Critical error in Server thread: " << e.what());
        }
    }

    std::unique_lock<std::shared_mutex> lock(m_ClientsMutex);
    for (const auto& client : m_Clients) {
        if (client->socket != INVALID_SOCKET) {
            shutdown(client->socket, SD_BOTH);
            closesocket(client->socket);
        }
    }
    m_Clients.clear();

    if (m_ListenSocket != INVALID_SOCKET) {
        closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
    }

    LOG_INFO(LOG_COMPONENT_SERVER, "Server thread stopped");
}

void ServerManager::Stop()
{
    if (!m_Running)
        return;

    m_Running = false;

    if (m_ServerThread.joinable()) {
        m_ServerThread.join();
    }

    if (m_AcceptThread.joinable()) {
        m_AcceptThread.join();
    }

    if (m_ListenSocket != INVALID_SOCKET) {
        closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
    }

    {
        std::unique_lock<std::shared_mutex> lock(m_ClientsMutex);
        for (auto& client : m_Clients) {
            if (client->socket != INVALID_SOCKET) {
                closesocket(client->socket);
                client->socket = INVALID_SOCKET;
            }
        }
        m_Clients.clear();
    }

    for (auto& thread : m_ClientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    m_ClientThreads.clear();

    LOG_INFO(LOG_COMPONENT_SERVER, "Server stopped");
}

void ServerManager::Update()
{
}

void ServerManager::AcceptThread()
{
    while (m_Running) {
        SOCKET clientSocket = accept(m_ListenSocket, nullptr, nullptr);
        
        if (clientSocket == INVALID_SOCKET) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                LOG_ERROR(LOG_COMPONENT_SERVER, "Accept failed: " << std::to_string(WSAGetLastError()));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        u_long mode = 1;
        if (ioctlsocket(clientSocket, FIONBIO, &mode) == SOCKET_ERROR) {
            LOG_ERROR(LOG_COMPONENT_SERVER, "Failed to set client socket to non-blocking mode");
            closesocket(clientSocket);
            continue;
        }

        DWORD timeout = SOCKET_TIMEOUT_MS;
        if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, 
                       reinterpret_cast<char*>(&timeout), sizeof(timeout)) == SOCKET_ERROR ||
            setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, 
                       reinterpret_cast<char*>(&timeout), sizeof(timeout)) == SOCKET_ERROR) {
            LOG_ERROR(LOG_COMPONENT_SERVER, "Failed to set socket timeouts");
            closesocket(clientSocket);
            continue;
        }

        auto client = std::make_shared<ClientInfo>();
        client->socket = clientSocket;
        client->isActive = true;
        client->lastHealthCheck = std::chrono::steady_clock::now();

        {
            std::unique_lock<std::shared_mutex> lock(m_ClientsMutex);
            if (m_Clients.size() >= MAX_CLIENTS) {
                LOG_ERROR(LOG_COMPONENT_SERVER, "Maximum number of clients reached");
                closesocket(clientSocket);
                continue;
            }
            m_Clients.push_back(client);
        }

        m_ClientThreads.emplace_back(&ServerManager::ClientThread, this, client);
    }
}

void ServerManager::ClientThread(std::shared_ptr<ClientInfo> client)
{
    try {
        while (m_Running && client->isActive) {
            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(client->socket, &readSet);

            timeval timeout{0, 1000};
            int result = select(0, &readSet, nullptr, nullptr, &timeout);

            if (result == SOCKET_ERROR) {
                LOG_ERROR(LOG_COMPONENT_SERVER, "Select failed for client: " << std::to_string(WSAGetLastError()));
                break;
            }

            if (result > 0) {
                int bytesReceived = recv(client->socket, 
                    client->recvBuffer.get() + client->recvBufferPos,
                    static_cast<int>(MAX_BUFFER_SIZE - client->recvBufferPos), 0);

                if (bytesReceived == SOCKET_ERROR) {
                    if (WSAGetLastError() != WSAEWOULDBLOCK) {
                        LOG_ERROR(LOG_COMPONENT_SERVER, "Receive failed: " << std::to_string(WSAGetLastError()));
                        break;
                    }
                }
                else if (bytesReceived == 0) {
                    break;
                }
                else {
                    client->recvBufferPos += bytesReceived;

                    size_t processedBytes = 0;
                    while (client->recvBufferPos - processedBytes >= sizeof(Packets::PacketHeader)) {
                        auto header = reinterpret_cast<Packets::PacketHeader*>(
                            client->recvBuffer.get() + processedBytes);
                        
                        size_t packetSize = 0;
                        switch (header->Type) {
                            case Packets::PacketType::Register:
                                packetSize = sizeof(Packets::RegisterPacket);
                                break;
                            case Packets::PacketType::HealthCheck:
                                packetSize = sizeof(Packets::HealthCheckPacket);
                                break;
                            case Packets::PacketType::MemoryResponse:
                                packetSize = sizeof(Packets::MemoryResponsePacket);
                                break;
                            case Packets::PacketType::SettingsRequest:
                                packetSize = sizeof(Packets::SettingsRequestPacket);
                                break;
                            default:
                                LOG_ERROR(LOG_COMPONENT_SERVER, "Unknown packet type received: " << std::to_string(static_cast<int>(header->Type)));
                                client->isActive = false;
                                return;
                        }

                        if (client->recvBufferPos - processedBytes < packetSize) {
                            break;
                        }

                        if (ProcessPacket(client, 
                            client->recvBuffer.get() + processedBytes,
                            packetSize)) {
                            processedBytes += packetSize;
                        }
                        else {
                            LOG_ERROR(LOG_COMPONENT_SERVER, "Failed to process packet from client: " << client->processName);
                            break;
                        }
                    }

                    if (processedBytes > 0) {
                        if (processedBytes < client->recvBufferPos) {
                            memmove(client->recvBuffer.get(),
                                client->recvBuffer.get() + processedBytes,
                                client->recvBufferPos - processedBytes);
                        }
                        client->recvBufferPos -= processedBytes;
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR(LOG_COMPONENT_SERVER, "Error in client thread: " << e.what());
    }

    if (client->socket != INVALID_SOCKET) {
        closesocket(client->socket);
        client->socket = INVALID_SOCKET;
    }
    client->isActive = false;
}

bool ServerManager::SendPacket(SOCKET socket, const void* data, size_t size)
{
    const char* buffer = static_cast<const char*>(data);
    size_t totalSent = 0;

    while (totalSent < size) {
        int result = send(socket, buffer + totalSent, 
            static_cast<int>(size - totalSent), 0);

        if (result == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        totalSent += result;
    }

    return true;
}

bool ServerManager::ProcessPacket(std::shared_ptr<ClientInfo> client, const char* data, size_t size)
{
    LOG_INFO(LOG_COMPONENT_SERVER, "Processing packet - Size: " << size);
    if (size < sizeof(Packets::PacketHeader)) {
        LOG_ERROR(LOG_COMPONENT_SERVER, "Received packet too small");
        return false;
    }

    auto header = reinterpret_cast<const Packets::PacketHeader*>(data);
    
    try {
        switch (header->Type) {
            case Packets::PacketType::Register:
                if (size >= sizeof(Packets::RegisterPacket)) {
                    auto packet = reinterpret_cast<const Packets::RegisterPacket*>(data);
                    
                    {
                        std::unique_lock<std::shared_mutex> lock(m_ClientsMutex);
                        auto it = std::find_if(m_Clients.begin(), m_Clients.end(),
                            [pid = packet->PID](const auto& c) { return c->pid == pid && c->isActive; });
                        
                        if (it != m_Clients.end()) {
                            LOG_INFO(LOG_COMPONENT_SERVER, "Removing existing client with PID: " << std::to_string(packet->PID));
                            (*it)->isActive = false;
                            if ((*it)->socket != INVALID_SOCKET) {
                                closesocket((*it)->socket);
                                (*it)->socket = INVALID_SOCKET;
                            }
                            m_Clients.erase(it);
                        }
                    }
                    
                    client->pid = packet->PID;
                    client->processName = packet->ClientName;
                    client->isActive = true;
                    client->UpdateActivity();
                    
                    LOG_INFO(LOG_COMPONENT_SERVER, "Registered new client: " << client->processName << " (PID: " << std::to_string(client->pid) << ")");
                    return true;
                }
                break;

            case Packets::PacketType::HealthCheck:
                if (size >= sizeof(Packets::HealthCheckPacket)) {
                    client->UpdateActivity();
                    return true;
                }
                break;

            case Packets::PacketType::MemoryResponse:
                LOG_INFO(LOG_COMPONENT_SERVER, "Memory Response Packet received - Size: " << size);
                if (size >= sizeof(Packets::MemoryResponsePacket)) {
                    auto packet = reinterpret_cast<const Packets::MemoryResponsePacket*>(data);
                    
                    try {
                        memcpy(&client->memoryInfo, &packet->State, sizeof(Packets::MemoryState));
                        client->UpdateActivity();
                        LOG_DEBUG(LOG_COMPONENT_SERVER, "Successfully processed memory response - Mob count: " 
                            << client->memoryInfo.MobListSize);
                    }
                    catch(const std::exception& e) {
                        LOG_ERROR(LOG_COMPONENT_SERVER, "Error copying memory state: " << e.what());
                        return false;
                    }
                    
                    return true;
                }
                break;

            case Packets::PacketType::SettingsRequest:
                if (size >= sizeof(Packets::SettingsRequestPacket)) {
                    Packets::SettingsResponsePacket response;
                    response.State = client->settings;
                    response.Header.Type = Packets::PacketType::SettingsResponse;
                    response.Header.Size = sizeof(response);
                    
                    LOG_INFO(LOG_COMPONENT_SERVER, "Sending settings to client: " << client->processName);
                    return SendPacket(client->socket, &response, sizeof(response));
                }
                break;

            case Packets::PacketType::Ack:
                return false;

            default:
                LOG_WARN(LOG_COMPONENT_SERVER, "Unknown packet type: " << static_cast<int>(header->Type));
                return false;
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR(LOG_COMPONENT_SERVER, "Error processing packet: " << e.what());
        return false;
    }

    return false;
}

bool ServerManager::RequestMemoryInfo(int clientPID)
{
    std::shared_lock<std::shared_mutex> lock(m_ClientsMutex);
    auto it = std::find_if(m_Clients.begin(), m_Clients.end(),
        [clientPID](const auto& client) { return client->pid == clientPID; });
    
    if (it == m_Clients.end() || !(*it)->isActive) {
        return false;
    }
    
    Packets::MemoryRequestPacket packet;
    packet.Header.Type = Packets::PacketType::MemoryRequest;
    packet.Header.Size = sizeof(packet);
    
    LOG_INFO(LOG_COMPONENT_SERVER, "Sending memory request to client: " << (*it)->processName);
    return SendPacket((*it)->socket, &packet, sizeof(packet));
}

const std::vector<std::shared_ptr<ClientInfo>>& ServerManager::GetConnectedClients() const
{
    return m_Clients;
}

const Packets::SettingsState* ServerManager::GetClientSettings(int clientPID) const
{
    std::shared_lock<std::shared_mutex> lock(m_ClientsMutex);
    auto it = std::find_if(m_Clients.begin(), m_Clients.end(),
        [clientPID](const auto& client) { return client->pid == clientPID; });
    
    if (it != m_Clients.end()) {
        return &(*it)->settings;
    }
    return nullptr;
}

ClientInfo* ServerManager::FindClientByPID(int clientPID)
{
    std::shared_lock<std::shared_mutex> lock(m_ClientsMutex);
    auto it = std::find_if(m_Clients.begin(), m_Clients.end(),
        [clientPID](const auto& client) { return client->pid == clientPID; });
    
    return it != m_Clients.end() ? it->get() : nullptr;
}

} // namespace Server