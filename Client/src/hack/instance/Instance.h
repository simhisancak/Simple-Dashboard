#pragma once
#include "FracqMath.hpp"
#include "common/Memory.hpp"
#include "hack/globals/Globals.h"
#include "PacketTypes.h"
#include <map>
#include <string>

namespace FracqClient {

typedef std::map<uint32_t, uintptr_t> CharacterInstanceMap;

class Instance {
private:
    uintptr_t m_Address;

public:
    Instance(uintptr_t address)
        : m_Address(address) { }

    bool IsValid() const { return getVID() > 100; }
    uintptr_t getAddress() const { return m_Address; }

    uint32_t getVID() const;
    Math::Vector3 getPixelPosition() const;
    void setPixelPosition(Math::Vector3 position) const;
    uint8_t getType() const;
    bool IsDead() const;
    std::string getName() const;
    uint32_t getRaceMotionData() const;
    uint32_t getMotionType() const;
    float getRotation() const;

    static Instance FromAddress(uintptr_t address) { return Instance(address); }
};

class InstanceHelper {
public:
    static Instance getMainActor();
    static CharacterInstanceMap getAlivaInstMap();
    static std::vector<Instance> getMobList(MobType targetTypes);
    static std::vector<Packets::Instance> getMobs(MobType targetTypes);
    static uintptr_t getInstanceByVID(uint32_t vid);
};

} // namespace FracqClient