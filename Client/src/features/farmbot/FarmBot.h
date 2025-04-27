#pragma once
#include <vector>

#include "../Features.h"
#include "FracqMath.hpp"
#include "PacketTypes.h"
#include "common/Logger.h"
#include "hack/helper/Helper.h"
#include "hack/instance/Instance.h"

namespace FracqClient {

// Forward declarations
class ClientApp;

class FarmBot : public Features {
public:
    virtual void Loop() override;

private:
    static Instance getAttackableMob(float distance, MobType targetTypes);
    static void WaitHack(MobType targetTypes);
    static void RangeDamage(MobType targetTypes, float distance);

    static bool m_FixedRange;
    static Math::Vector3 m_FixedRangePos;
};

} // namespace FracqClient
