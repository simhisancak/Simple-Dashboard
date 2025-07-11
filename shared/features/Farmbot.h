#pragma once
#include "BasePackets.h"

namespace Packets {
struct FarmBotState {
    bool FarmBotStatus;
    bool AutoLoot;
    float AreaSize;
    MobType TargetTypes;
    bool RenderSkip;
    bool ClearRam;
    bool WaitHack;

    FarmBotState()
        : FarmBotStatus(false)
        , AutoLoot(false)
        , AreaSize(0.0f)
        , TargetTypes(MobType::None)
        , RenderSkip(false)
        , ClearRam(false)
        , WaitHack(false) { }
};
} // namespace Packets