#pragma once
#include <cstdint>
#include "common/Memory.hpp"

class Globals
{
private:
    static Globals *instance;
    Globals() = default;

public:
    Globals(const Globals &) = delete;
    Globals &operator=(const Globals &) = delete;

    static Globals *Get()
    {
        if (!instance)
        {
            instance = new Globals();
        }
        return instance;
    }

    void Initialize()
    {
        // PythonCharacterManager = Memory::FindAndResolvePointer("metin2client.exe", "8b 0d ? ? ? ? e9 ? ? ? ? cc cc cc cc cc 8b 0d ? ? ? ? e9 ? ? ? ? cc cc cc cc cc 8b d1");
        // PythonCharacterManager = Memory::Read<uintptr_t>(PythonCharacterManager);

        // PythonPlayer = Memory::FindAndResolvePointer("metin2client.exe", "8b 0d ? ? ? ? e8 ? ? ? ? e8 ? ? ? ? 8b e5 5d c3 cc cc cc cc 8b 0d ? ? ? ? 83 c1");
        // PythonPlayer = Memory::Read<uintptr_t>(PythonPlayer);

        // RenderCondition = Memory::FindPattern("metin2client.exe", "c7 05 ? ? ? ? ? ? ? ? eb ? 85 d2");
        // RenderCondition = RenderCondition + 0xE;

        PythonCharacterManager = Memory::FindAndResolvePointer("metin2client.exe", "8b 0d ? ? ? ? e9 ? ? ? ? cc cc cc cc cc 8b 0d ? ? ? ? e9 ? ? ? ? cc cc cc cc cc 8b d1");
        PythonCharacterManager = Memory::Read<uintptr_t>(PythonCharacterManager);

        PythonPlayer = Memory::FindAndResolvePointer("metin2client.exe", "8b 0d ? ? ? ? e8 ? ? ? ? e8 ? ? ? ? 8b e5 5d c3 cc cc cc cc 8b 0d ? ? ? ? 83 c1");
        PythonPlayer = Memory::Read<uintptr_t>(PythonPlayer);

        RenderCondition = Memory::FindPattern("metin2client.exe", "c7 05 ? ? ? ? ? ? ? ? eb ? 85 d2");
        RenderCondition = RenderCondition + 0xE;

        D3D9Device = Memory::FindAndResolvePointer("metin2client.exe", "a1 ? ? ? ? 6a ? 6a ? 50 8b 08 ff 51", 0x1);
        //D3D9Device = Memory::Read<uintptr_t>(D3D9Device);
    }

    uint32_t OffsetBase = 0x250;
    uint32_t NameOffset = 0x10;
    uint32_t m_kAliveInstMapOffset = 0x20;
    uint32_t SetTargetOffset = 0x60;

    uint32_t PosXOffset = OffsetBase + 0x468;
    uint32_t PosYOffset = PosXOffset + 0x4;
    uint32_t PosZOffset = PosXOffset + 0x8;
    uint32_t InstanceTypeOffset = OffsetBase + 0x3FC;
    uint32_t IsDeadOffset = OffsetBase + 0x444;
    uint32_t VIDOffset = OffsetBase + 0x55C;

    uintptr_t PythonCharacterManager = 0;
    uintptr_t PythonPlayer = 0;
    uintptr_t RenderCondition = 0;
    uintptr_t PythonResourceManager = 0;
    uintptr_t PythonResourceDestory = 0;
    uintptr_t D3D9Device = 0;
};
