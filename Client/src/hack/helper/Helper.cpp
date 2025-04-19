#include "Helper.h"
#include "../globals/Globals.h"
#include <algorithm>

bool Helper::CompareInstances(const Instance& a, const Instance& b)
{
    auto posA = a.GetPixelPosition();
    auto posB = b.GetPixelPosition();
    auto mainActor = GetMainActor();
    auto mainPos = mainActor.GetPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

bool Helper::ComparePacketsInstances(const Packets::Instance& a, const Packets::Instance& b)
{
    auto posA = a.Position;
    auto posB = b.Position;
    auto mainActor = GetMainActor();
    auto mainPos = mainActor.GetPixelPosition();

    return mainPos.DistanceTo(posA) < mainPos.DistanceTo(posB);
}

Instance Helper::GetMainActor()
{
    return Instance::FromAddress(Memory::Read<uintptr_t>(Globals::Get()->PythonCharacterManager + 0xC));
}

TCharacterInstanceMap Helper::getAlivaInstMap()
{
    uintptr_t m_kAliveInstMap_p = Memory::Read<uintptr_t>(Globals::Get()->PythonCharacterManager + Globals::Get()->m_kAliveInstMapOffset);
    if (m_kAliveInstMap_p < 0x10000) {
        return TCharacterInstanceMap();
    }

    uintptr_t m_kAliveInstMap_map = Memory::Read<uintptr_t>(m_kAliveInstMap_p + 4);
    if (m_kAliveInstMap_map < 0x10000) {
        return TCharacterInstanceMap();
    }

    return *(TCharacterInstanceMap*)(m_kAliveInstMap_map);
}

std::vector<Packets::Instance> Helper::getMobs(MobType targetTypes)
{
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

std::vector<Instance> Helper::getMobList(MobType targetTypes)
{
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

void Helper::setTargetVid(uint32_t vid)
{
    Memory::Write<uint32_t>(Globals::Get()->PythonPlayer + Globals::Get()->SetTargetOffset, vid);
}

void Helper::RenderCondition(bool enable)
{
    const BYTE enable_bytes[] = { 0x90, 0xE9 };
    const BYTE disable_bytes[] = { 0x0F, 0x85 };

    if (enable) {
        Memory::PatchBytes(Globals::Get()->RenderCondition, enable_bytes, sizeof(enable_bytes));
        return;
    }

    Memory::PatchBytes(Globals::Get()->RenderCondition, disable_bytes, sizeof(disable_bytes));
}

bool Helper::ClearRam()
{
    // Get device pointer similar to ImGui's approach
    if (!Globals::Get()->D3D9Device)
        return false;

    LPDIRECT3DDEVICE9 device = nullptr;
    device = *(LPDIRECT3DDEVICE9*)Globals::Get()->D3D9Device;
    if (!device)
        return false;

    // Save current state
    IDirect3DStateBlock9* state_block = nullptr;
    device->CreateStateBlock(D3DSBT_ALL, &state_block);
    if (state_block)
        state_block->Capture();

    // Simple viewport test
    D3DVIEWPORT9 vp;
    vp.X = 0;
    vp.Y = 0;
    vp.Width = 100;
    vp.Height = 100;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    device->SetViewport(&vp);

    // Restore state
    if (state_block) {
        state_block->Apply();
        state_block->Release();
    }

    // Clear working set
    HANDLE process = GetCurrentProcess();
    SetProcessWorkingSetSize(process, -1, -1);

    // Try to minimize the working set
    SIZE_T minWS = 8 * 1024 * 1024; // 8 MB minimum
    SIZE_T maxWS = 128 * 1024 * 1024; // 128 MB maximum
    SetProcessWorkingSetSize(process, minWS, maxWS);

    LOG_INFO(LOG_COMPONENT_CLIENTAPP, "Device test completed");
    return true;
}