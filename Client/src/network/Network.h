#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <psapi.h>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include "PacketTypes.h"

class ClientApp; // Forward declaration

namespace Network
{

    class NetworkClient
    {
    public:
        NetworkClient(ClientApp *clientApp);
        ~NetworkClient();

        bool Initialize();
        bool Connect(const std::string &ip, uint16_t port);
        void Disconnect();
        bool IsConnected() const { return m_Connected; }

        bool SendRegisterPacket(const Packets::RegisterPacket &packet);
        bool SendHealthCheckPacket();
        bool SendSettingsRequestPacket(const Packets::SettingsRequestPacket &packet);

    private:
        static constexpr int SOCKET_TIMEOUT_MS = 5000;
        static constexpr size_t MAX_BUFFER_SIZE = 65536;

        void ProcessingThread();
        bool ProcessPacket(const char *data, size_t size);
        bool SendPacketInternal(const void *data, size_t size);
        void CleanupWinsock();

        SOCKET m_Socket;
        bool m_Connected;
        bool m_Initialized;
        ClientApp *m_ClientApp;

        std::thread m_ProcessingThread;
        std::atomic<bool> m_Running;
        std::unique_ptr<char[]> m_RecvBuffer;
        size_t m_RecvBufferPos;
        std::mutex m_SendMutex;
    };

}