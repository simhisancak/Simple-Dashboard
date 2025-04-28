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

    static void RenderCondition(bool enable);
    static bool ClearRam();
    static std::vector<Math::Vector3>
    DivideTwoPointsByDistance(float distance, Math::Vector3 pointStart, Math::Vector3 pointEnd);

    static void FilterByAreaSize(std::vector<Instance>& mobList,
                                 const Math::Vector3& mainActorPos,
                                 float areaSize);

    static Math::Vector3
    CalculateCircularMovement(const Math::Vector3& center, float radius, float angle, float step);
    static bool IsMobNearPosition(const Math::Vector3& position,
                                  const std::vector<Instance>& mobList,
                                  float threshold);

    static Math::Vector3
    ClampDistance(const Math::Vector3& origin, const Math::Vector3& target, float maxDistance);

    static float CalculateRotation(const Math::Vector3& from, const Math::Vector3& to) {
        Math::Vector3 direction = to - from;
        float angle = atan2f(direction.x, direction.y);

        // Radyanı dereceye çevir (180/π ≈ 57.2958)
        angle = angle * 57.2958f;

        // Negatif açıları 0-360 aralığına çevir
        if (angle < 0) {
            angle += 360.0f;
        }

        return angle;
    }
};

} // namespace FracqClient