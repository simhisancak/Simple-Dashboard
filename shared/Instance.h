#pragma once
#include "BasePackets.h"
#include "FracqMath.hpp"

namespace Packets {

struct Instance {
    uint32_t VID;
    char Name[64];
    Math::Vector3 Position;
    uint8_t Type;

    Instance()
        : VID(0)
        , Name("")
        , Position(0, 0, 0)
        , Type(0) { }
};
}