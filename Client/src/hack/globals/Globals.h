#pragma once
#include <cstdint>

#include "common/Memory.hpp"

namespace FracqClient {

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
        PythonCharacterManager = Common::Memory::FindAndResolvePointer(
            "metin2client.exe",
            "a1 ? ? ? ? 89 45 ? 8b 4d ? e8 ? ? ? ? 8b e5 5d c3 cc cc cc 55 8b ec 83 ec ? 56",
            1);
        PythonCharacterManager = Common::Memory::Read<uintptr_t>(PythonCharacterManager);

        PythonPlayer = Common::Memory::FindAndResolvePointer(
            "metin2client.exe",
            "a1 ? ? ? ? 83 c0 ? 89 45 ? 8b 4d ? 8b 11 8b 4d ? 8b 82 ? ? ? ? ff d0 89 45 ? 83 7d ? "
            "? 75 ? 6a ? 68 ? ? ? ? ff 15 ? ? ? ? 83 c4 ? eb ? 8b 4d ? e8 ? ? ? ? 50 68 ? ? ? ? ff "
            "15 ? ? ? ? 83 c4 ? 8b e5 5d c3 cc cc cc cc cc cc cc cc cc 55 8b ec 83 ec ? c6 45 ? ? "
            "8d 45 ? 50 6a ? 8b 4d ? 51 e8 ? ? ? ? 83 c4 ? 0f b6 ? 85 d2 75 ? 6a ? e8 ? ? ? ? 83 "
            "c4 ? eb ? c7 45",
            1);
        PythonPlayer = Common::Memory::Read<uintptr_t>(PythonPlayer);

        PythonNetworkStream = Common::Memory::FindAndResolvePointer(
            "metin2client.exe",
            "a1 ? ? ? ? 89 45 ? 8b 4d ? 8b 51 ? 89 55 ? 8b 4d",
            1);

        PythonNetworkStream = Common::Memory::Read<uintptr_t>(PythonNetworkStream);

        ItemManager = Common::Memory::FindAndResolvePointer(
            "metin2client.exe",
            "8b 0d ? ? ? ? e8 ? ? ? ? a1 ? ? ? ? 89 45 ? 8b 4d ? "
            "e8 ? ? ? ? 89 45 ? 83 7d ? ? 75 ? 0f b7 4d");
        ItemManager = Common::Memory::Read<uintptr_t>(ItemManager);

        PythonItem = Common::Memory::FindAndResolvePointer(
            "metin2client.exe",
            "a1 ? ? ? ? 89 45 ? 8d 4d ? 51 8b 55 ? 52 8b 4d ? e8 ? ? ? ? 0f b6 ? 85 c0 75 ? e9",
            1);
        PythonItem = Common::Memory::Read<uintptr_t>(PythonItem);

        //--------------------------------

        RenderCondition = Common::Memory::FindPattern("metin2client.exe",
                                                      "83 3d ? ? ? ? ? 0f 85 ? ? ? ? 51");
        RenderCondition = RenderCondition + 0x7;

        SendAttackPacket
            = Common::Memory::FindAndResolveCall("metin2client.exe",
                                                 "e8 ? ? ? ? eb ? 6a ? 8b 4d ? e8 ? ? ? ? b0");

        SendCharacterStatePacket = Common::Memory::FindAndResolveCall(
            "metin2client.exe",
            "e8 ? ? ? ? 8b e5 5d c2 ? ? cc cc cc cc cc cc cc cc cc cc cc cc cc cc cc 55 8b ec 83 "
            "ec ? 89 4d ? a1 ? ? ? ? 89 45 ? 6a");

        SendClickItemPacket = Common::Memory::FindAndResolveCall(
            "metin2client.exe",
            "e8 ? ? ? ? 8b 4d ? e8 ? ? ? ? 8b 4d ? e8 ? ? ? ? eb ? 8d 4d");

        GetNameString = Common::Memory::FindAndResolveCall(
            "metin2client.exe",
            "e8 ? ? ? ? 50 68 ? ? ? ? ff 15 ? ? ? ? 83 c4 ? 8b e5 5d c3 cc cc cc cc 55 8b ec 83 ec "
            "? 8d 45 ? 50 6a ? 8b 4d ? 51 e8 ? ? ? ? 83 c4 ? 0f b6 ? 85 d2 75 ? 6a ? e8 ? ? ? ? 83 "
            "c4 ? eb ? a1 ? ? ? ? 83 c0 ? 89 45 ? 8b 4d ? 51 8b 55 ? 8b 02 8b 4d ? 8b 50 ? ff d2 "
            "89 45");
    }

    uint32_t OffsetBase = 0x2C4;
    uint32_t NameOffset = 0x14;
    uint32_t m_kAliveInstMapOffset = 0x20;

    uint32_t SetAttackVidOffset = 0x349AC;
    uint32_t SetAttackStateOffset = 0x64;

    uint32_t ItemMapOffset = 0x4;
    uint32_t ItemNameOffset = 0xED;
    uint32_t ItemVnumOffset = 0xCC;

    uint32_t GroundItemMapOffset = 0x4;
    uint32_t GroundItemPosXOffset = 0x8;
    uint32_t GroundItemPosYOffset = GroundItemPosXOffset + 0x4;
    uint32_t GroundItemOwnershipOffset = 0x300;
    uint32_t GroundItemVnumOffset = 0x4;

    uint32_t TargetVidOffset = 0x34A50;

    uint32_t PosXOffset = OffsetBase + 0x4CC;
    uint32_t PosYOffset = PosXOffset + 0x4;
    uint32_t PosZOffset = PosXOffset + 0x8;
    uint32_t InstanceTypeOffset = OffsetBase + 0x44C;
    uint32_t IsDeadOffset = OffsetBase + 0x498;
    uint32_t VIDOffset = OffsetBase + 0x5F0;

    uintptr_t PythonCharacterManager = 0;
    uintptr_t PythonPlayer = 0;
    uintptr_t PythonNetworkStream = 0;
    uintptr_t ItemManager = 0;
    uintptr_t PythonItem = 0;

    uintptr_t RenderCondition = 0;

    uintptr_t SendAttackPacket = 0;
    uintptr_t SendCharacterStatePacket = 0;
    uintptr_t SendClickItemPacket = 0;
    uintptr_t GetNameString = 0;
};

} // namespace FracqClient
