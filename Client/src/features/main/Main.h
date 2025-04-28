#pragma once
#include <vector>

#include "features/Features.h"
#include "FracqMath.hpp"
#include "PacketTypes.h"
#include "common/Logger.h"
#include "hack/helper/Helper.h"
#include "hack/instance/Instance.h"

namespace FracqClient {

// Forward declarations
class ClientApp;

class Main : public Features {

    static const float safeDistance;
    static const float radius;
    static const float angleStep;

public:
    virtual void Loop() override;

private:
    static void WaitDamage();
    static void RangeDamage();
    static void RangeDamageSafe();

    static std::vector<GroundItem> FilterGroundItems(const std::vector<GroundItem>& groundItemList,
                                                     const Math::Vector3& mainActorPos);
    static void PickupGroundItems();
    static void MoveSpeedHack();

    static Math::Vector3 FindSafePosition(const Math::Vector3& mainActorPos,
                                          float currentRadius,
                                          float currentAngle,
                                          float currentStep,
                                          const std::vector<Instance>& mobList);

    static Packets::MainState m_settings;
    static Packets::PickupState m_pickupSettings;

    static Math::Vector3 m_mainActorPosLast;
    static float m_angle;
    static bool m_boost;
};

} // namespace FracqClient