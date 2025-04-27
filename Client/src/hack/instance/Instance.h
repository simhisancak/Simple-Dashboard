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

    bool IsValid() const { return GetVID() > 100; }
    uintptr_t GetAddress() const { return m_Address; }

    uint32_t GetVID() const;
    Math::Vector3 GetPixelPosition() const;
    uint8_t GetType() const;
    bool IsDead() const;
    std::string GetName() const;

    static Instance FromAddress(uintptr_t address) { return Instance(address); }
};

class InstanceHelper {
public:
    static Instance GetMainActor();
    static CharacterInstanceMap getAlivaInstMap();
    static std::vector<Instance> getMobList(MobType targetTypes);
    static std::vector<Packets::Instance> getMobs(MobType targetTypes);
    static uintptr_t GetInstanceByVID(uint32_t vid);
};

} // namespace FracqClient