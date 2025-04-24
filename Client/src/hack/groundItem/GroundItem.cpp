#include "GroundItem.h"
#include <windows.h>

std::string GroundItem::GetOwnership() const {
    if (!IsValid()) {
        return "";
    }

    const char* ansiStr = reinterpret_cast<const char*>(
        m_Address + Globals::Get()->GroundItemOwnershipOffset);

    wchar_t wideBuffer[65];
    MultiByteToWideChar(1254, 0, ansiStr, -1, wideBuffer, 65);

    char utf8Buffer[195];
    WideCharToMultiByte(CP_UTF8, 0, wideBuffer, -1, utf8Buffer, sizeof(utf8Buffer), NULL, NULL);

    return std::string(utf8Buffer);
}

Math::Vector3 GroundItem::GetPixelPosition() const {
    if (!IsValid()) {
        return Math::Vector3();
    }

    return Math::Vector3(
        abs(Memory::Read<float>(m_Address + Globals::Get()->GroundItemPosXOffset) / 100.0f),
        abs(Memory::Read<float>(m_Address + Globals::Get()->GroundItemPosYOffset) / 100.0f),
        0);
}