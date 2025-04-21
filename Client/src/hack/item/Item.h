#pragma once
#include "FracqMath.hpp"
#include "common/Memory.hpp"
#include "../globals/Globals.h"
#include <string>

class Item {
private:
    uintptr_t m_Address;

public:
    Item(uintptr_t address)
        : m_Address(address) { }

    bool IsValid() const { return m_Address > 0x1000; }

    std::string GetName() const;

    static Item FromAddress(uintptr_t address) { return Item(address); }
};