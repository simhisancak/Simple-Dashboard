#pragma once
#include "BasePackets.h"

namespace Packets {

enum class DamageType : uint8_t { WaitDamage, RangeDamage, RangeDamageSafe };

struct MainState {
    float AreaSize;
    MobType TargetTypes;
    bool RenderSkip;
    bool ClearRam;
    bool DamageEnabled;
    bool Pickup;
    bool MoveSpeedEnabled;
    int MoveSpeed;
    DamageType DamageType;
    uint32_t DamageDelay;
    uint32_t DamageLimit;

    MainState()
        : AreaSize(20.0f)
        , TargetTypes(MobType::None)
        , RenderSkip(false)
        , ClearRam(false)
        , DamageEnabled(false)
        , Pickup(false)
        , MoveSpeedEnabled(false)
        , MoveSpeed(40)
        , DamageType(DamageType::WaitDamage)
        , DamageDelay(200)
        , DamageLimit(10) { }
};
} // namespace Packets