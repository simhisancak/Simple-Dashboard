#pragma once

#include "BasePackets.h"
#include "features/Farmbot.h"
#include "features/Main.h"
#include "features/Pickup.h"

namespace Packets {
struct SettingsState {
    FarmBotState FarmBot;
    MainState Main;
    PickupState Pickup;

    SettingsState()
        : FarmBot()
        , Main()
        , Pickup() { }
};

struct SettingsRequestPacket {
    PacketHeader Header;

    SettingsRequestPacket()
        : Header(PacketType::SettingsRequest, sizeof(SettingsRequestPacket)) { }
};

struct SettingsResponsePacket {
    PacketHeader Header;
    SettingsState State;

    SettingsResponsePacket()
        : Header(PacketType::SettingsResponse, sizeof(SettingsResponsePacket)) { }
};
}