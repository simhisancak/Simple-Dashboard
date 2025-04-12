#include "Network.h"
#include "core/ClientApp.h"
#include "common/Logger.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

namespace Network {

NetworkClient::NetworkClient(ClientApp* clientApp)
    : m_ClientApp(clientApp)
    , m_Socket(INVALID_SOCKET)
    , m_Connected(false)
    , m_Initialized(false)
    , m_Running(false)
    , m_RecvBufferPos(0)
{
    m_RecvBuffer = std::make_unique<char[]>(MAX_BUFFER_SIZE);
}

NetworkClient::~NetworkClient()
{
    m_Running = false;
    if (m_ProcessingThread.joinable()) {
        m_ProcessingThread.join();
    }
    Disconnect();
    CleanupWinsock();
}

bool NetworkClient::Initialize()
{
    if (m_Initialized) {
        return true;
    }

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        LOG_ERROR(LOG_COMPONENT_NETWORK, "WSAStartup failed: " << result);
        return false;
    }

    m_Initialized = true;
    return true;
}

bool NetworkClient::Connect(const std::string& ip, uint16_t port)
{
    if (!m_Initialized && !Initialize()) {
        return false;
    }

    if (m_Connected) {
        Disconnect();
    }

    m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_Socket == INVALID_SOCKET) {
        LOG_ERROR(LOG_COMPONENT_NETWORK, "Socket creation failed: " << WSAGetLastError());
        return false;
    }

    u_long mode = 1;
    if (ioctlsocket(m_Socket, FIONBIO, &mode) == SOCKET_ERROR) {
        LOG_ERROR(LOG_COMPONENT_NETWORK, "Failed to set non-blocking mode");
        closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
        return false;
    }

    DWORD timeout = SOCKET_TIMEOUT_MS;
    if (setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, 
                   reinterpret_cast<char*>(&timeout), sizeof(timeout)) == SOCKET_ERROR ||
        setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO, 
                   reinterpret_cast<char*>(&timeout), sizeof(timeout)) == SOCKET_ERROR) {
        LOG_ERROR(LOG_COMPONENT_NETWORK, "Failed to set socket timeouts");
        closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) != 1) {
        LOG_ERROR(LOG_COMPONENT_NETWORK, "Invalid IP address");
        closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
        return false;
    }

    if (connect(m_Socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            LOG_ERROR(LOG_COMPONENT_NETWORK, "Connect failed: " << WSAGetLastError());
            closesocket(m_Socket);
            m_Socket = INVALID_SOCKET;
            return false;
        }

        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(m_Socket, &writeSet);

        timeval tv;
        tv.tv_sec = SOCKET_TIMEOUT_MS / 1000;
        tv.tv_usec = (SOCKET_TIMEOUT_MS % 1000) * 1000;

        if (select(0, nullptr, &writeSet, nullptr, &tv) <= 0) {
            LOG_ERROR(LOG_COMPONENT_NETWORK, "Connection timeout");
            closesocket(m_Socket);
            m_Socket = INVALID_SOCKET;
            return false;
        }
    }

    LOG_INFO(LOG_COMPONENT_NETWORK, "Connected to " << ip << ":" << port);
    m_Connected = true;
    m_Running = true;
    m_ProcessingThread = std::thread(&NetworkClient::ProcessingThread, this);
    
    return true;
}

void NetworkClient::ProcessingThread()
{
    while (m_Running) {
        if (!m_Connected) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(m_Socket, &readSet);

        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 10000;

        int result = select(0, &readSet, nullptr, nullptr, &tv);
        if (result == SOCKET_ERROR) {
            LOG_ERROR(LOG_COMPONENT_NETWORK, "Select failed: " << WSAGetLastError());
            m_Connected = false;
            continue;
        }

        if (result > 0) {
            int received = recv(m_Socket, 
                m_RecvBuffer.get() + m_RecvBufferPos,
                static_cast<int>(MAX_BUFFER_SIZE - m_RecvBufferPos), 0);

            if (received == SOCKET_ERROR) {
                if (WSAGetLastError() != WSAEWOULDBLOCK) {
                    LOG_ERROR(LOG_COMPONENT_NETWORK, "Receive failed: " << WSAGetLastError());
                    m_Connected = false;
                }
                continue;
            }

            if (received == 0) {
                LOG_ERROR(LOG_COMPONENT_NETWORK, "Connection closed by server");
                m_Connected = false;
                continue;
            }

            m_RecvBufferPos += received;

            size_t processedBytes = 0;
            while (m_RecvBufferPos - processedBytes >= sizeof(Packets::PacketHeader)) {
                auto header = reinterpret_cast<Packets::PacketHeader*>(
                    m_RecvBuffer.get() + processedBytes);

                size_t packetSize = 0;
                switch (header->Type) {
                    case Packets::PacketType::MemoryRequest:
                        packetSize = sizeof(Packets::MemoryRequestPacket);
                        break;
                    case Packets::PacketType::SettingsResponse:
                        packetSize = sizeof(Packets::SettingsResponsePacket);
                        break;
                    default:
                        LOG_WARN(LOG_COMPONENT_NETWORK, "Unknown packet type: " << static_cast<int>(header->Type));
                        m_Connected = false;
                        return;
                }

                if (m_RecvBufferPos - processedBytes < packetSize) {
                    break;
                }

                ProcessPacket(m_RecvBuffer.get() + processedBytes, packetSize);
                processedBytes += packetSize;
            }

            if (processedBytes > 0) {
                if (processedBytes < m_RecvBufferPos) {
                    memmove(m_RecvBuffer.get(),
                        m_RecvBuffer.get() + processedBytes,
                        m_RecvBufferPos - processedBytes);
                }
                m_RecvBufferPos -= processedBytes;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

bool NetworkClient::ProcessPacket(const char* data, size_t size)
{
    if (!data || size < sizeof(Packets::PacketHeader)) {
        LOG_ERROR(LOG_COMPONENT_NETWORK, "Invalid packet data or size");
        return false;
    }

    auto header = reinterpret_cast<const Packets::PacketHeader*>(data);

    if (size > MAX_BUFFER_SIZE) {
        LOG_ERROR(LOG_COMPONENT_NETWORK, "Packet size exceeds maximum buffer size");
        return false;
    }

    try {
        switch (header->Type) {
            case Packets::PacketType::MemoryRequest:
            {
                if (size != sizeof(Packets::MemoryRequestPacket)) {
                    LOG_ERROR(LOG_COMPONENT_NETWORK, "Invalid MemoryRequest packet size");
                    return false;
                }

                LOG_DEBUG(LOG_COMPONENT_NETWORK, "Processing memory request");
                
                Packets::MemoryResponsePacket response;
                response.State = m_ClientApp->GetMemoryState();
                
                LOG_DEBUG(LOG_COMPONENT_NETWORK, "Sending memory response - Size: " << sizeof(response));
                return SendPacketInternal(&response, sizeof(response));
            }
            case Packets::PacketType::SettingsResponse:
            {
                if (size != sizeof(Packets::SettingsResponsePacket)) {
                    LOG_ERROR(LOG_COMPONENT_NETWORK, "Invalid SettingsResponse packet size");
                    return false;
                }

                auto settingsPacket = reinterpret_cast<const Packets::SettingsResponsePacket*>(data);
                return m_ClientApp->ProcessSettingsPacket(*settingsPacket);
            }
            default:
                LOG_ERROR(LOG_COMPONENT_NETWORK, "Unknown packet type: " << static_cast<int>(header->Type));
                return false;
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR(LOG_COMPONENT_NETWORK, "Exception while processing packet: " << e.what());
        return false;
    }
}

bool NetworkClient::SendRegisterPacket(const Packets::RegisterPacket& packet)
{
    std::lock_guard<std::mutex> lock(m_SendMutex);
    return SendPacketInternal(&packet, sizeof(packet));
}

bool NetworkClient::SendHealthCheckPacket()
{
    std::lock_guard<std::mutex> lock(m_SendMutex);
    Packets::HealthCheckPacket packet;
    packet.PID = GetCurrentProcessId();
    packet.Timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return SendPacketInternal(&packet, sizeof(packet));
}

bool NetworkClient::SendPacketInternal(const void* data, size_t size)
{
    if (!m_Connected || m_Socket == INVALID_SOCKET) {
        LOG_ERROR(LOG_COMPONENT_NETWORK, "Not connected to server");
        return false;
    }

    const char* buffer = static_cast<const char*>(data);
    size_t totalSent = 0;

    while (totalSent < size) {
        int sent = send(m_Socket, buffer + totalSent, 
            static_cast<int>(size - totalSent), 0);

        if (sent == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                LOG_ERROR(LOG_COMPONENT_NETWORK, "Send failed: " << WSAGetLastError());
                m_Connected = false;
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        totalSent += sent;
    }

    return true;
}

bool NetworkClient::SendSettingsRequestPacket(const Packets::SettingsRequestPacket& packet)
{
    std::lock_guard<std::mutex> lock(m_SendMutex);
    return SendPacketInternal(&packet, sizeof(packet));
}

void NetworkClient::Disconnect()
{
    std::lock_guard<std::mutex> lock(m_SendMutex);
    
    if (m_Socket != INVALID_SOCKET) {
        shutdown(m_Socket, SD_BOTH);
        closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
    }
    
    m_Connected = false;
}

void NetworkClient::CleanupWinsock()
{
    if (m_Initialized) {
        WSACleanup();
        m_Initialized = false;
    }
}

}