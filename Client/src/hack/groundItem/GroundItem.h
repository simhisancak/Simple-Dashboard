#pragma once
#include "FracqMath.hpp"
#include "common/Memory.hpp"
#include "../globals/Globals.h"
#include <string>
#include <map>

namespace FracqClient {

typedef std::map<uint32_t, uintptr_t> GroundItemMap;

class GroundItem {
private:
    uintptr_t m_Address;
    uint32_t m_VID;

public:
    GroundItem(uintptr_t address, uint32_t VID)
        : m_Address(address)
        , m_VID(VID) { }

    bool IsValid() const { return m_Address > 0x1000; }

    uint32_t getVID() const { return m_VID; }
    uintptr_t getAddress() const { return m_Address; }
    uint32_t getVnum() const;

    std::string getOwnership() const;
    static GroundItem FromAddress(uintptr_t address, uint32_t VID) {
        return GroundItem(address, VID);
    }
    Math::Vector3 getPixelPosition() const;
};

class GroundItemHelper {
public:
    static GroundItemMap getGroundItemMap();
    static std::vector<GroundItem> getGroundItemList();
};

} // namespace FracqClient