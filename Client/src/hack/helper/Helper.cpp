#include "Helper.h"

#include <algorithm>

#include "../globals/Globals.h"

bool Helper::CompareInstances(const Instance& a, const Instance& b) {
    auto posA = a.GetPixelPosition();
    auto posB = b.GetPixelPosition();
    auto mainActor = GetMainActor();
    auto mainPos = mainActor.GetPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

bool Helper::ComparePacketsInstances(const Packets::Instance& a, const Packets::Instance& b) {
    auto posA = a.Position;
    auto posB = b.Position;
    auto mainActor = GetMainActor();
    auto mainPos = mainActor.GetPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

Instance Helper::GetMainActor() {
    return Instance::FromAddress(
        Memory::Read<uintptr_t>(Globals::Get()->PythonCharacterManager + 0xC));
}

TCharacterInstanceMap Helper::getAlivaInstMap() {
    uintptr_t m_kAliveInstMap_p = Memory::Read<uintptr_t>(Globals::Get()->PythonCharacterManager
                                                          + Globals::Get()->m_kAliveInstMapOffset);
    if (m_kAliveInstMap_p < 0x10000) {
        return TCharacterInstanceMap();
    }

    uintptr_t m_kAliveInstMap_map = Memory::Read<uintptr_t>(m_kAliveInstMap_p + 0x4);
    if (m_kAliveInstMap_map < 0x10000) {
        return TCharacterInstanceMap();
    }

    return *(TCharacterInstanceMap*)(m_kAliveInstMap_map);
}

std::vector<Packets::Instance> Helper::getMobs(MobType targetTypes) {
    std::vector<Packets::Instance> mobList;
    auto mainActor = GetMainActor();

    if (!mainActor.IsValid())
        return mobList;

    TCharacterInstanceMap m_kAliveInstMap = getAlivaInstMap();

    for (auto itor = m_kAliveInstMap.begin(); itor != m_kAliveInstMap.end(); itor++) {
        uint32_t iIndex = itor->first;
        auto instance = Instance::FromAddress(itor->second);
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
        packet.Name = instance.GetName();
        packet.Position = pos;
        packet.Type = type;

        mobList.push_back(packet);
    }
    return mobList;
}

std::vector<Instance> Helper::getMobList(MobType targetTypes) {
    std::vector<Instance> mobList;
    auto mainActor = GetMainActor();

    if (!mainActor.IsValid())
        return mobList;

    TCharacterInstanceMap m_kAliveInstMap = getAlivaInstMap();

    for (auto itor = m_kAliveInstMap.begin(); itor != m_kAliveInstMap.end(); itor++) {
        uint32_t iIndex = itor->first;
        auto instance = Instance::FromAddress(itor->second);

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

void Helper::setAttackVid(uint32_t vid) {
    Memory::Write<uint32_t>(Globals::Get()->PythonPlayer + Globals::Get()->SetAttackVidOffset, vid);
}

void Helper::setAttackState(bool state) {
    uint32_t _state = state ? 3 : 0;

    Memory::Write<uint32_t>(Globals::Get()->PythonPlayer + Globals::Get()->SetAttackStateOffset,
                            _state);
}

uint32_t Helper::getTargetVid() {
    return Memory::Read<uint32_t>(Globals::Get()->PythonPlayer + Globals::Get()->TargetVidOffset);
}

void Helper::RenderCondition(bool enable) {
    const BYTE enable_bytes[] = { 0x90, 0xE9 };
    const BYTE disable_bytes[] = { 0x0F, 0x85 };

    if (enable) {
        Memory::PatchBytes(Globals::Get()->RenderCondition, enable_bytes, sizeof(enable_bytes));
        return;
    }

    Memory::PatchBytes(Globals::Get()->RenderCondition, disable_bytes, sizeof(disable_bytes));
}

bool Helper::ClearRam() {
    HANDLE process = GetCurrentProcess();
    SetProcessWorkingSetSize(process, -1, -1);

    SIZE_T minWS = 8 * 1024 * 1024; // 8 MB minimum
    SIZE_T maxWS = 128 * 1024 * 1024; // 128 MB maximum
    SetProcessWorkingSetSize(process, minWS, maxWS);

    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Ram cleared");
    return true;
}

std::vector<Math::Vector3> Helper::DivideTwoPointsByDistance(float distance,
                                                             Math::Vector3 pointStart,
                                                             Math::Vector3 pointEnd) {
    std::vector<Math::Vector3> points;
    float totalDistance = pointStart.DistanceTo(pointEnd);

    if (totalDistance <= distance) {
        points.push_back(pointEnd);
        return points;
    }

    int steps = static_cast<int>(totalDistance / distance);
    float stepRatio = distance / totalDistance;

    for (int i = 1; i <= steps; i++) {
        points.push_back(pointStart + (pointEnd - pointStart) * (stepRatio * i));
    }

    if (steps * distance < totalDistance) {
        points.push_back(pointEnd);
    }

    return points;
}

void Helper::SendAttackPacket(uint32_t vid) {
    uintptr_t netptr = Globals::Get()->PythonNetworkStream;
    uintptr_t attackcall = Globals::Get()->SendAttackPacket;

    __asm {
        mov ecx, netptr
        push vid
        push 0
        call attackcall
    }
}

void Helper::SendCharacterStatePacket(Math::Vector3* pos,
                                      float rot,
                                      uint32_t eFunc,
                                      uint32_t uArg) {
    DWORD sendstatecall = Globals::Get()->SendCharacterStatePacket;
    DWORD netptr = Globals::Get()->PythonNetworkStream;
    Math::Vector3 _pos = *pos;
    _pos.x = _pos.x * 100.0f;
    _pos.y = _pos.y * 100.0f;

    __asm {
        mov ecx, netptr
        push uArg
        push eFunc
        push rot
        lea eax, _pos
        push eax
        call sendstatecall
    }
}