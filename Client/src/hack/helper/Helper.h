#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <winternl.h>

#include <map>

#include "../instance/Instance.h"
#include "../item/Item.h"
#include "../groundItem/GroundItem.h"
#include "common/Logger.h"
#include "features/Farmbot.h"
#include "PacketTypes.h"

namespace FracqClient {

class Helper {
public:
    static bool CompareInstances(const Instance& a, const Instance& b);
    static bool ComparePacketsInstances(const Packets::Instance& a, const Packets::Instance& b);
    static bool CompareGroundItems(const GroundItem& a, const GroundItem& b);
    static void setAttackVid(uint32_t vid);
    static void setAttackState(bool state);
    static uint32_t getTargetVid();

    static void RenderCondition(bool enable);
    static bool ClearRam();
    static std::vector<Math::Vector3>
    DivideTwoPointsByDistance(float distance, Math::Vector3 pointStart, Math::Vector3 pointEnd);
    static void SendAttackPacket(uint32_t vid);
    static void
    SendCharacterStatePacket(Math::Vector3* pos, float rot, uint32_t eFunc, uint32_t uArg);
    static void SendClickItemPacket(uint32_t vid);
    static void MoveTo(const Math::Vector3& fromPos, const Math::Vector3& toPos);
    static std::vector<Instance> FilterByAreaSize(const std::vector<Instance>& mobList,
                                                  const Math::Vector3& mainActorPos,
                                                  float areaSize);

    static Math::Vector3
    CalculateCircularMovement(const Math::Vector3& center, float radius, float angle, float step);
    static bool IsMobNearPosition(const Math::Vector3& position,
                                  const std::vector<Instance>& mobList,
                                  float threshold);

    static Math::Vector3
    ClampDistance(const Math::Vector3& origin, const Math::Vector3& target, float maxDistance);
};

} // namespace FracqClient