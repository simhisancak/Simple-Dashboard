#pragma once
#include <vector>

#include "../Features.h"
#include "FracqMath.hpp"
#include "PacketTypes.h"
#include "common/Logger.h"
#include "hack/helper/Helper.h"
#include "hack/instance/Instance.h"

// Forward declarations
class ClientApp;

class Main : public Features {
public:
    virtual void Loop() override;

private:
    static void WaitHack();
    static void RangeDamage();
    static void PickupGroundItems();

    static Packets::MainState m_settings;
};
