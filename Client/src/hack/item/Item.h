#pragma once
#include "FracqMath.hpp"
#include "common/Memory.hpp"
#include "../globals/Globals.h"
#include <string>
#include <map>
namespace FracqClient {

typedef std::map<uint32_t, uintptr_t> ItemMap;

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

class ItemHelper {
public:
    static ItemMap getItemMap();
    static std::vector<Item> getItemList();
};

} // namespace FracqClient