#pragma once
#include "BasePackets.h"
#include "FracqMath.hpp"
#include <string>

namespace Packets
{
    struct Instance
    {
        uint32_t VID;
        std::string Name;
        Math::Vector3 Position;
        uint8_t Type;

        Instance()
            : VID(0),
              Name(""),
              Position(0, 0, 0),
              Type(0)
        {
        }
    };
}