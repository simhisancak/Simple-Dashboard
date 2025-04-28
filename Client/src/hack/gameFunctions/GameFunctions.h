#pragma once

#include <cstdint>
#include "FracqMath.hpp"

namespace FracqClient {

class GameFunctions {
public:
    static void SendAttackPacket(uint32_t vid);
    static void
    SendCharacterStatePacket(Math::Vector3 pos, float rot, uint32_t eFunc, uint32_t uArg);
    static void SendClickItemPacket(uint32_t vid);
    static uintptr_t getInstanceByVID(uint32_t vid);
};

} // namespace FracqClient
