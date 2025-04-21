#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>

enum class MobType : uint8_t {
    None = 0,
    Normal = 1 << 0,
    NPC = 1 << 1,
    Metin = 1 << 2,
    Player = 1 << 6,
    All = Normal | Metin
};

inline MobType operator|(MobType a, MobType b) {
    return static_cast<MobType>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline MobType operator&(MobType a, MobType b) {
    return static_cast<MobType>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

struct MobTypeUI {
    const char* name;
    MobType type;
};

namespace Packets {

enum class PacketType : uint8_t {
    Register = 0x01,
    Heartbeat = 0x02,
    Disconnect = 0x03,
    HealthCheck = 0x04,
    Ack = 0x05,

    MemoryRequest = 0x06,
    MemoryResponse = 0x07,

    SettingsRequest = 0x08,
    SettingsResponse = 0x09
};

struct PacketHeader {
    PacketType Type;
    uint32_t Size;

    PacketHeader(PacketType type = PacketType::Register, uint32_t size = 0)
        : Type(type)
        , Size(size) { }
};

struct RegisterPacket {
    PacketHeader Header;
    int32_t PID;
    char ClientName[64];

    RegisterPacket()
        : Header(PacketType::Register, sizeof(RegisterPacket))
        , PID(0) {
        memset(ClientName, 0, sizeof(ClientName));
    }
};

struct HealthCheckPacket {
    PacketHeader Header;
    int32_t PID;
    uint64_t Timestamp;

    HealthCheckPacket()
        : Header(PacketType::HealthCheck, sizeof(HealthCheckPacket))
        , PID(0)
        , Timestamp(0) { }
};

struct AckPacket {
    PacketHeader Header;

    AckPacket()
        : Header(PacketType::Ack, sizeof(AckPacket)) { }
};
}