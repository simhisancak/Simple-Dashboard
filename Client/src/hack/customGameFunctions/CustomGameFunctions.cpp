#include "CustomGameFunctions.h"
#include "common/Memory.hpp"
#include "hack/globals/Globals.h"
#include "hack/helper/Helper.h"
#include "hack/gameFunctions/GameFunctions.h"

namespace FracqClient {

bool CustomGameFunctions::m_boost = false;

void CustomGameFunctions::setAttackVid(uint32_t vid) {
    Common::Memory::Write<uint32_t>(Globals::Get()->PythonPlayer
                                        + Globals::Get()->SetAttackVidOffset,
                                    vid);
}

void CustomGameFunctions::setAttackState(bool state) {
    uint32_t _state = state ? 3 : 0;

    Common::Memory::Write<uint32_t>(Globals::Get()->PythonPlayer
                                        + Globals::Get()->SetAttackStateOffset,
                                    _state);
}

uint32_t CustomGameFunctions::getTargetVid() {
    return Common::Memory::Read<uint32_t>(Globals::Get()->PythonPlayer
                                          + Globals::Get()->TargetVidOffset);
}

void CustomGameFunctions::MoveTo(const Math::Vector3& fromPos,
                                 const Math::Vector3& toPos,
                                 float rotation) {

    if (fromPos.DistanceTo(toPos) < 5) {
        SendMovementPacket(toPos, rotation);
        return;
    }

    auto points = Helper::DivideTwoPointsByDistance(6, fromPos, toPos);
    for (auto& point : points) {
        SendMovementPacket(point, rotation);
    }
}

void CustomGameFunctions::SendMovementPacket(const Math::Vector3& position, float rotation) {
    // Boost durumuna göre farklı paket gönder
    if (m_boost) {
        GameFunctions::SendCharacterStatePacket(position, rotation, 1, 0);
        m_boost = false;
    } else {
        GameFunctions::SendCharacterStatePacket(position, rotation, 0, 0);
        m_boost = true;
    }
}

} // namespace FracqClient
