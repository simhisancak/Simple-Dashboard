#pragma once
#include "FracqMath.hpp"
#include "common/Memory.hpp"
#include "../globals/Globals.h"
#include <string>

namespace FracqClient {

class Instance {
private:
    uintptr_t m_Address;

public:
    Instance(uintptr_t address)
        : m_Address(address) { }

    bool IsValid() const { return GetVID() > 100; }
    uintptr_t GetAddress() const { return m_Address; }

    uint32_t GetVID() const;
    Math::Vector3 GetPixelPosition() const;
    uint8_t GetType() const;
    bool IsDead() const;
    std::string GetName() const;

    static Instance FromAddress(uintptr_t address) { return Instance(address); }
};

} // namespace FracqClient