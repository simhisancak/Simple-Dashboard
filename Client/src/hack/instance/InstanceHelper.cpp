#include "Instance.h"

namespace FracqClient {

namespace {
    __declspec(noinline) bool ReadAliveMap(uintptr_t map_address, CharacterInstanceMap& out_map) {
        __try {
            out_map = *reinterpret_cast<CharacterInstanceMap*>(map_address);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }
}

Instance InstanceHelper::GetMainActor() {
    return Instance::FromAddress(
        Common::Memory::Read<uintptr_t>(Globals::Get()->PythonCharacterManager + 0xC));
}

CharacterInstanceMap InstanceHelper::getAlivaInstMap() {
    uintptr_t m_kAliveInstMap_p = Common::Memory::Read<uintptr_t>(
        Globals::Get()->PythonCharacterManager + Globals::Get()->m_kAliveInstMapOffset);
    if (m_kAliveInstMap_p < 0x10000) {
        return CharacterInstanceMap();
    }

    uintptr_t m_kAliveInstMap_map = Common::Memory::Read<uintptr_t>(m_kAliveInstMap_p + 0x4);
    if (m_kAliveInstMap_map < 0x10000) {
        return CharacterInstanceMap();
    }

    CharacterInstanceMap result;
    return ReadAliveMap(m_kAliveInstMap_map, result) ? result : CharacterInstanceMap();
}

std::vector<Packets::Instance> InstanceHelper::getMobs(MobType targetTypes) {
    std::vector<Packets::Instance> mobList;
    auto mainActor = GetMainActor();

    if (!mainActor.IsValid())
        return mobList;

    CharacterInstanceMap m_kAliveInstMap = getAlivaInstMap();

    for (const auto& Ins : m_kAliveInstMap) {
        uint32_t iIndex = Ins.first;
        auto instance = Instance::FromAddress(Ins.second);
        auto packet = Packets::Instance();

        if (!instance.IsValid() || instance.GetAddress() == mainActor.GetAddress())
            continue;

        uint8_t type = instance.GetType();
        if (!static_cast<uint8_t>(targetTypes & static_cast<MobType>(1 << type)))
            continue;

        if (instance.IsDead())
            continue;

        auto pos = instance.GetPixelPosition();
        if (pos.x < 10.0f || pos.y < 10.0f)
            continue;

        packet.VID = iIndex;
        strcpy(packet.Name, instance.GetName().c_str());
        packet.Position = pos;
        packet.Type = type;

        mobList.push_back(packet);
    }
    return mobList;
}

std::vector<Instance> InstanceHelper::getMobList(MobType targetTypes) {
    std::vector<Instance> mobList;
    auto mainActor = GetMainActor();

    if (!mainActor.IsValid())
        return mobList;

    CharacterInstanceMap m_kAliveInstMap = getAlivaInstMap();

    for (const auto& Ins : m_kAliveInstMap) {
        uint32_t iIndex = Ins.first;
        auto instance = Instance::FromAddress(Ins.second);

        if (!instance.IsValid() || instance.GetAddress() == mainActor.GetAddress())
            continue;

        if (!static_cast<uint8_t>(targetTypes & static_cast<MobType>(1 << instance.GetType())))
            continue;

        if (instance.IsDead())
            continue;

        auto pos = instance.GetPixelPosition();
        if (pos.x < 10.0f || pos.y < 10.0f)
            continue;

        mobList.push_back(instance);
    }

    return mobList;
}

uintptr_t InstanceHelper::GetInstanceByVID(uint32_t vid) {
    uintptr_t charactermanager = Globals::Get()->PythonCharacterManager;
    uintptr_t _ptr = 0;

    __try {
        __asm {
            mov ecx, charactermanager
            push vid
            add ecx, 4
            mov eax, [ecx]
            call [eax + 8]
            mov _ptr, eax
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }

    return _ptr;
}

} // namespace FracqClient