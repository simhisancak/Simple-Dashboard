#pragma once

#include <Windows.h>
#include <cstdint>
#include <array>
#include "BasePackets.h"
#include "Instance.h"

namespace Packets {
constexpr size_t MAX_ITEM_LIST_SIZE = 1024;
constexpr size_t MAX_NAME_LENGTH = 64;

struct Item {
    uint32_t Vnum;
    char Name[MAX_NAME_LENGTH];
};

struct ItemDumpState {
    size_t ItemListSize;
    Item ItemList[MAX_ITEM_LIST_SIZE];

    ItemDumpState()
        : ItemListSize(0)
        , ItemList() { }
};

struct ItemDumpRequestPacket {
    PacketHeader Header;
    char Filter[MAX_NAME_LENGTH];

    ItemDumpRequestPacket()
        : Header(PacketType::ItemDumpRequest, sizeof(ItemDumpRequestPacket))
        , Filter() { }
};

struct ItemDumpResponsePacket {
    PacketHeader Header;
    ItemDumpState State;

    ItemDumpResponsePacket()
        : Header(PacketType::ItemDumpResponse, sizeof(ItemDumpResponsePacket)) { }
};
}
