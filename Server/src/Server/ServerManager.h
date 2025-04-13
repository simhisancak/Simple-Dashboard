#pragma once

#include <winsock2.h>
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
#include "Memory.h"
#include <chrono>
#include <unordered_map>
#include <queue>
#include <condition_variable>
#include "../common/Logger.h"

class Application;

namespace Server
{

    constexpr size_t MAX_BUFFER_SIZE = 65536;
    constexpr int SOCKET_TIMEOUT_MS = 5000;
    constexpr int MAX_CLIENTS = 1000;
    constexpr int HEALTH_CHECK_INTERVAL_MS = 5000;
    constexpr int HEALTH_CHECK_TIMEOUT_MS = 7500;
    constexpr int MEMORY_REQUEST_INTERVAL_MS = 50;

    struct ClientInfo
    {
        SOCKET socket;
        int pid;
        std::string processName;
        std::string timestamp;
        bool isActive;
        std::string lastError;
        std::chrono::steady_clock::time_point lastHealthCheck;
        std::chrono::steady_clock::time_point lastActivity;
        std::unique_ptr<char[]> recvBuffer;
        size_t recvBufferSize;
        size_t recvBufferPos;
        Packets::SettingsState settings;
        Packets::MemoryState memoryInfo;

        ClientInfo()
            : socket(INVALID_SOCKET), pid(0), isActive(false), recvBuffer(new char[MAX_BUFFER_SIZE]), recvBufferSize(0), recvBufferPos(0)
        {
            lastHealthCheck = std::chrono::steady_clock::now();
            lastActivity = lastHealthCheck;
            ZeroMemory(&settings, sizeof(settings));
        }

        ClientInfo(const ClientInfo &) = delete;
        ClientInfo &operator=(const ClientInfo &) = delete;
        ClientInfo(ClientInfo &&) = default;
        ClientInfo &operator=(ClientInfo &&) = default;

        void UpdateActivity()
        {
            lastActivity = std::chrono::steady_clock::now();
            lastHealthCheck = lastActivity;
        }

        bool IsTimedOut() const
        {
            auto now = std::chrono::steady_clock::now();
            auto healthCheckDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
                                       now - lastHealthCheck)
                                       .count();
            auto activityDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    now - lastActivity)
                                    .count();

            return !isActive ||
                   healthCheckDiff > HEALTH_CHECK_TIMEOUT_MS ||
                   activityDiff > (HEALTH_CHECK_TIMEOUT_MS * 2);
        }
    };

    template <typename T>
    class MessageQueue
    {
        std::queue<T> queue;
        mutable std::mutex mutex;
        std::condition_variable cv;

    public:
        void push(T &&item)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(item));
            cv.notify_one();
        }

        bool try_pop(T &item)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.empty())
                return false;
            item = std::move(queue.front());
            queue.pop();
            return true;
        }
    };

    class ServerManager
    {
    public:
        ServerManager(Application *app);
        ~ServerManager();

        bool Start(int port);
        void Stop();
        void Update();

        const std::vector<std::shared_ptr<ClientInfo>> &GetConnectedClients() const;
        bool SendSettingsToClient(int clientPID, const Packets::SettingsResponsePacket &settings);
        bool RequestMemoryInfo(int clientPID);
        const Packets::SettingsState *GetClientSettings(int clientPID) const;
        bool UpdateClientSettings(int clientPID, const Packets::SettingsResponsePacket &settings);
        ClientInfo *FindClientByPID(int clientPID);
        const ClientInfo *FindClientByPID(int clientPID) const;

    private:
        void ServerThread();
        void AcceptThread();
        void ClientThread(std::shared_ptr<ClientInfo> client);
        void ProcessRegisterRequest(SOCKET clientSocket, const char *buffer, int bytesReceived);
        void RemoveClient(SOCKET clientSocket);
        bool SendPacket(SOCKET socket, const void *data, size_t size);
        bool ReceivePacket(SOCKET socket, void *data, size_t size);
        bool ProcessPacket(std::shared_ptr<ClientInfo> client, const char *data, size_t size);

    private:
        Application *m_App;
        std::atomic<bool> m_Running;
        SOCKET m_ListenSocket;
        int m_Port;
        std::thread m_ServerThread;
        std::thread m_AcceptThread;
        std::vector<std::thread> m_ClientThreads;
        mutable std::shared_mutex m_ClientsMutex;
        std::vector<std::shared_ptr<ClientInfo>> m_Clients;
        MessageQueue<std::pair<int, Packets::SettingsResponsePacket>> m_SettingsQueue;
        MessageQueue<int> m_MemoryRequestQueue;
    };

}