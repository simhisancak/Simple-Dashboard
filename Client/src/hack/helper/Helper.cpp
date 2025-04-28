#include "Helper.h"

#include <algorithm>

#include "../globals/Globals.h"
#include "common/Helper.h"
#include "hack/gameFunctions/GameFunctions.h"

namespace FracqClient {

bool Helper::CompareInstances(const Instance& a, const Instance& b) {
    auto posA = a.getPixelPosition();
    auto posB = b.getPixelPosition();
    auto mainActor = InstanceHelper::getMainActor();
    auto mainPos = mainActor.getPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

bool Helper::ComparePacketsInstances(const Packets::Instance& a, const Packets::Instance& b) {
    auto posA = a.Position;
    auto posB = b.Position;
    auto mainActor = InstanceHelper::getMainActor();
    auto mainPos = mainActor.getPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

bool Helper::CompareGroundItems(const GroundItem& a, const GroundItem& b) {
    auto posA = a.getPixelPosition();
    auto posB = b.getPixelPosition();
    auto mainActor = InstanceHelper::getMainActor();
    auto mainPos = mainActor.getPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

void Helper::RenderCondition(bool enable) {
    const BYTE enable_bytes[] = { 0x90, 0xE9 };
    const BYTE disable_bytes[] = { 0x0F, 0x85 };

    if (enable) {
        Common::Memory::PatchBytes(Globals::Get()->RenderCondition,
                                   enable_bytes,
                                   sizeof(enable_bytes));
        return;
    }

    Common::Memory::PatchBytes(Globals::Get()->RenderCondition,
                               disable_bytes,
                               sizeof(disable_bytes));
}

bool Helper::ClearRam() {
    HANDLE process = GetCurrentProcess();
    SetProcessWorkingSetSize(process, -1, -1);

    SIZE_T minWS = 8 * 1024 * 1024; // 8 MB minimum
    SIZE_T maxWS = 128 * 1024 * 1024; // 128 MB maximum
    SetProcessWorkingSetSize(process, minWS, maxWS);

    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Ram cleared");
    return true;
}

std::vector<Math::Vector3> Helper::DivideTwoPointsByDistance(float distance,
                                                             Math::Vector3 pointStart,
                                                             Math::Vector3 pointEnd) {
    std::vector<Math::Vector3> points;
    float totalDistance = pointStart.DistanceTo(pointEnd);

    if (totalDistance <= distance) {
        points.push_back(pointEnd);
        return points;
    }

    int steps = static_cast<int>(totalDistance / distance);
    float stepRatio = distance / totalDistance;

    for (int i = 1; i <= steps; i++) {
        points.push_back(pointStart + (pointEnd - pointStart) * (stepRatio * i));
    }

    if (steps * distance < totalDistance) {
        points.push_back(pointEnd);
    }

    return points;
}

void Helper::FilterByAreaSize(std::vector<Instance>& mobList,
                              const Math::Vector3& mainActorPos,
                              float areaSize) {
    mobList.erase(std::remove_if(mobList.begin(),
                                 mobList.end(),
                                 [&mainActorPos, areaSize](const Instance& mob) {
                                     return mainActorPos.DistanceTo(mob.getPixelPosition())
                                            > areaSize;
                                 }),
                  mobList.end());
}

Math::Vector3
Helper::ClampDistance(const Math::Vector3& origin, const Math::Vector3& target, float maxDistance) {
    auto direction = target - origin;
    float distance = direction.Length();
    if (distance > maxDistance && distance > 0.0001f) {
        direction = direction * (maxDistance / distance);
        return origin + direction;
    }
    return target;
}

Math::Vector3 Helper::CalculateCircularMovement(const Math::Vector3& center,
                                                float radius,
                                                float angle,
                                                float step) {
    float newAngle = angle + step;
    float x = center.x + radius * cos(newAngle);
    float y = center.y + radius * sin(newAngle);
    return Math::Vector3(x, y, center.z);
}

bool Helper::IsMobNearPosition(const Math::Vector3& position,
                               const std::vector<Instance>& mobList,
                               float threshold) {
    for (const auto& mob : mobList) {
        if (position.DistanceTo(mob.getPixelPosition()) < threshold) {
            return true;
        }
    }
    return false;
}

} // namespace FracqClient