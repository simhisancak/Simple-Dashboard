#pragma once

#include "BasePackets.h"
#include "features/Farmbot.h"

namespace Packets{
    struct SettingsState {
        FarmBotState FarmBot;

        SettingsState()
            : FarmBot()
        {}
    };

    struct SettingsRequestPacket {
        PacketHeader Header;
        
        SettingsRequestPacket()
            : Header(PacketType::SettingsRequest, sizeof(SettingsRequestPacket))
        {}
    };

    struct SettingsResponsePacket {
        PacketHeader Header;
        SettingsState State;
        
        SettingsResponsePacket()
            : Header(PacketType::SettingsResponse, sizeof(SettingsResponsePacket))
        {}
    };
}