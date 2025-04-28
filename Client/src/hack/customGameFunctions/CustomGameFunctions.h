#pragma once

#include <cstdint>
#include "FracqMath.hpp"
namespace FracqClient {

class CustomGameFunctions {
public:
    static void setAttackVid(uint32_t vid);
    static void setAttackState(bool state);
    static uint32_t getTargetVid();
    static void MoveTo(const Math::Vector3& fromPos, const Math::Vector3& toPos);
};

} // namespace FracqClient
