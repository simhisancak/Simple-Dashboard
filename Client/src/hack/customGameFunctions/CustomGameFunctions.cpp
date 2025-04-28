#include "CustomGameFunctions.h"
#include "common/Memory.hpp"
#include "hack/globals/Globals.h"
#include "hack/helper/Helper.h"
#include "hack/gameFunctions/GameFunctions.h"

namespace FracqClient {

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

void CustomGameFunctions::MoveTo(const Math::Vector3& fromPos, const Math::Vector3& toPos) {
    auto points = Helper::DivideTwoPointsByDistance(5, fromPos, toPos);
    for (auto& point : points) {
        GameFunctions::SendCharacterStatePacket(point, 0, 0, 0);
    }
}

} // namespace FracqClient
