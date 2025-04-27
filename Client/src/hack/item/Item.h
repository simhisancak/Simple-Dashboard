#pragma once
#include "FracqMath.hpp"
#include "common/Memory.hpp"
#include "../globals/Globals.h"
#include <string>

namespace FracqClient {

class Item {
private:
    uintptr_t m_Address;

public:
    Item(uintptr_t address)
        : m_Address(address) { }

    bool IsValid() const { return !GetName().empty(); }

    std::string GetName() const;
    uint32_t GetVnum() const;

    static Item FromAddress(uintptr_t address) { return Item(address); }
};

} // namespace FracqClient