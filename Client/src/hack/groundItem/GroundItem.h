#pragma once
#include "FracqMath.hpp"
#include "common/Memory.hpp"
#include "../globals/Globals.h"
#include <string>

class GroundItem {
private:
    uintptr_t m_Address;
    uint32_t m_VirtualNumber;

public:
    GroundItem(uintptr_t address, uint32_t virtualNumber)
        : m_Address(address)
        , m_VirtualNumber(virtualNumber) { }

    bool IsValid() const { return m_Address > 0x1000; }

    uint32_t GetVID() const { return m_VirtualNumber; }
    uintptr_t GetAddress() const { return m_Address; }

    std::string GetOwnership() const;
    static GroundItem FromAddress(uintptr_t address, uint32_t virtualNumber) {
        return GroundItem(address, virtualNumber);
    }
    Math::Vector3 GetPixelPosition() const;
};