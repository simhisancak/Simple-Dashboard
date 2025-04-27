#pragma once

#include <array>
#include "BasePackets.h"
#include "Instance.h"

namespace Packets {
constexpr size_t MAX_MOB_LIST_SIZE = 1024;

struct MemoryState {
    HANDLE hproc;
    uint64_t ModuleBase;
    uint32_t ModuleSize;
    Instance MainActor;
    size_t MobListSize;
    std::array<Instance, MAX_MOB_LIST_SIZE> MobList;

    MemoryState()
        : hproc(INVALID_HANDLE_VALUE)
        , ModuleBase(0)
        , ModuleSize(0)
        , MainActor()
        , MobListSize(0)
        , MobList() { }
};

struct MemoryRequestPacket {
    PacketHeader Header;

    MemoryRequestPacket()
        : Header(PacketType::MemoryRequest, sizeof(MemoryRequestPacket)) { }
};

struct MemoryResponsePacket {
    PacketHeader Header;
    MemoryState State;

    MemoryResponsePacket()
        : Header(PacketType::MemoryResponse, sizeof(MemoryResponsePacket)) { }
};
}
