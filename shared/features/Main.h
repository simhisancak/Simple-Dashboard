#pragma once
#include "BasePackets.h"

namespace Packets {

enum class DamageType : uint8_t { None, WaitHack, RangeDamage };

struct MainState {
    float AreaSize;
    MobType TargetTypes;
    bool RenderSkip;
    bool ClearRam;
    bool DamageEnabled;
    bool Pickup;
    DamageType DamageType;
    uint32_t DamageDelay;
    uint32_t DamageCount;
    uint32_t DamageLimit;

    MainState()
        : AreaSize(20.0f)
        , TargetTypes(MobType::None)
        , RenderSkip(false)
        , ClearRam(false)
        , DamageEnabled(false)
        , Pickup(false)
        , DamageType(DamageType::None)
        , DamageDelay(200)
        , DamageCount(3)
        , DamageLimit(10) { }
};
} // namespace Packets