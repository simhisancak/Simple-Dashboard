#pragma once
#include "common/Logger.h"
#include "FracqMath.hpp"
#include "PacketTypes.h"
#include "../Features.h"
#include "hack/instance/Instance.h"
#include "hack/helper/Helper.h"
#include <vector>



// Forward declarations
class ClientApp;

class FarmBot : public Features {
public:    
    virtual void Loop() override;
   
private:
    static Instance getAttackableMob(float distance, MobType targetTypes);

    static bool m_FixedRange;
    static Math::Vector3 m_FixedRangePos;
};
