#include "Helper.h"

#include <algorithm>

#include "../globals/Globals.h"
#include "common/Helper.h"
namespace FracqClient {

bool Helper::CompareInstances(const Instance& a, const Instance& b) {
    auto posA = a.GetPixelPosition();
    auto posB = b.GetPixelPosition();
    auto mainActor = InstanceHelper::GetMainActor();
    auto mainPos = mainActor.GetPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

bool Helper::ComparePacketsInstances(const Packets::Instance& a, const Packets::Instance& b) {
    auto posA = a.Position;
    auto posB = b.Position;
    auto mainActor = InstanceHelper::GetMainActor();
    auto mainPos = mainActor.GetPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

bool Helper::CompareGroundItems(const GroundItem& a, const GroundItem& b) {
    auto posA = a.GetPixelPosition();
    auto posB = b.GetPixelPosition();
    auto mainActor = InstanceHelper::GetMainActor();
    auto mainPos = mainActor.GetPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

void Helper::setAttackVid(uint32_t vid) {
    Common::Memory::Write<uint32_t>(Globals::Get()->PythonPlayer
                                        + Globals::Get()->SetAttackVidOffset,
                                    vid);
}

void Helper::setAttackState(bool state) {
    uint32_t _state = state ? 3 : 0;

    Common::Memory::Write<uint32_t>(Globals::Get()->PythonPlayer
                                        + Globals::Get()->SetAttackStateOffset,
                                    _state);
}

uint32_t Helper::getTargetVid() {
    return Common::Memory::Read<uint32_t>(Globals::Get()->PythonPlayer
                                          + Globals::Get()->TargetVidOffset);
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

void Helper::SendAttackPacket(uint32_t vid) {
    uintptr_t netptr = Globals::Get()->PythonNetworkStream;
    uintptr_t attackcall = Globals::Get()->SendAttackPacket;

    __try {
        __asm {
            mov ecx, netptr
            push vid
            push 0
            call attackcall
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // Handle exception silently
    }
}

void Helper::SendCharacterStatePacket(Math::Vector3* pos,
                                      float rot,
                                      uint32_t eFunc,
                                      uint32_t uArg) {
    uintptr_t sendstatecall = Globals::Get()->SendCharacterStatePacket;
    uintptr_t netptr = Globals::Get()->PythonNetworkStream;
    Math::Vector3 _pos = *pos;
    _pos.x = _pos.x * 100.0f;
    _pos.y = _pos.y * 100.0f;

    __try {
        __asm {
            mov ecx, netptr
            push uArg
            push eFunc
            push rot
            lea eax, _pos
            push eax
            call sendstatecall
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // Handle exception silently
    }
}

void Helper::SendClickItemPacket(uint32_t vid) {
    uintptr_t pythonplayer = Globals::Get()->PythonPlayer;
    uintptr_t clickitemcall = Globals::Get()->SendClickItemPacket;

    __try {
        __asm {
            mov ecx, pythonplayer
            push vid
            call clickitemcall
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // Handle exception silently
    }
}

void Helper::MoveTo(const Math::Vector3& fromPos, const Math::Vector3& toPos) {
    auto points = DivideTwoPointsByDistance(2, fromPos, toPos);
    for (auto& point : points) {
        SendCharacterStatePacket(&point, 0, 0, 0);
        Sleep(3);
    }
}

std::vector<Instance> Helper::FilterByAreaSize(const std::vector<Instance>& mobList,
                                               const Math::Vector3& mainActorPos,
                                               float areaSize) {
    std::vector<Instance> filtered;
    for (const auto& mob : mobList) {
        if (mainActorPos.DistanceTo(mob.GetPixelPosition()) <= areaSize) {
            filtered.push_back(mob);
        }
    }
    return filtered;
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
        if (position.DistanceTo(mob.GetPixelPosition()) < threshold) {
            return true;
        }
    }
    return false;
}

} // namespace FracqClient