#pragma once
#include <cstdint>
#include "common/Memory.hpp"
#include "common/Logger.h"

class Globals {
private:
    static Globals* instance;
    Globals() = default;

public:
    Globals(const Globals&) = delete;
    Globals& operator=(const Globals&) = delete;

    static Globals* Get() {
        if (!instance) {
            instance = new Globals();
        }
        return instance;
    }

    void Initialize() {
        PythonCharacterManager = Memory::FindAndResolvePointer("metin2client.exe", "8b 0d ? ? ? ? e9 ? ? ? ? cc cc cc cc cc 8b 0d ? ? ? ? e9 ? ? ? ? cc cc cc cc cc 8b d1");
        PythonCharacterManager = Memory::Read<uintptr_t>(PythonCharacterManager);

        PythonPlayer = Memory::FindAndResolvePointer("metin2client.exe", "8b 0d ? ? ? ? e8 ? ? ? ? e8 ? ? ? ? 8b e5 5d c3 cc cc cc cc 8b 0d ? ? ? ? 83 c1");
        PythonPlayer = Memory::Read<uintptr_t>(PythonPlayer);

        MobList = Memory::FindAndResolvePointer("metin2client.exe", "8b 0d ? ? ? ? 83 c2 ? 89 15 ? ? ? ? 80 7e");
        MobList = Memory::Read<uintptr_t>(MobList);

        RenderCondition = Memory::FindPattern("metin2client.exe", "c7 05 ? ? ? ? ? ? ? ? eb ? 85 d2");
        RenderCondition = RenderCondition + 0xF;

        PythonResourceManager = Memory::FindAndResolvePointer("metin2client.exe", "c7 01 ? ? ? ? c7 05 ? ? ? ? ? ? ? ? c3 cc cc cc cc cc cc cc cc cc cc cc cc cc cc cc 55 8b ec 6a ? 68 ? ? ? ? 64 a1 ? ? ? ? 50 51 56 a1 ? ? ? ? 33 c5 50 8d 45 ? 64 a3 ? ? ? ? 8b f1 89 75 ? c7 06 ? ? ? ? 8d 4e");
        PythonResourceManager = Memory::Read<uintptr_t>(PythonResourceManager);

        PythonResourceDestory = Memory::FindAndResolveCall("metin2client.exe", "e8 ? ? ? ? 8b 47 ? 8d 4f ? 50 ff 30 8d 45 ? 50 e8 ? ? ? ? ff 77 ? e8 ? ? ? ? 8b 47 ? 8d 4f ? 83 c4 ? c6 45");
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
    uintptr_t MobList = 0;
    uintptr_t RenderCondition = 0;
    uintptr_t PythonResourceManager = 0;
    uintptr_t PythonResourceDestory = 0;
};
