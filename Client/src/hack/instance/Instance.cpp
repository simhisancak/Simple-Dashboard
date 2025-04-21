#include "Instance.h"
#include <windows.h>

uint32_t Instance::GetVID() const {
    if (m_Address < 0x1000) {
        return 0;
    }

    uint32_t vid = Memory::Read<uint32_t>(m_Address + Globals::Get()->VIDOffset);
    return vid > 100 ? vid : 0;
}

Math::Vector3 Instance::GetPixelPosition() const {
    if (!IsValid()) {
        return Math::Vector3();
    }
    return Math::Vector3(abs(Memory::Read<float>(m_Address + Globals::Get()->PosXOffset) / 100.0f),
                         abs(Memory::Read<float>(m_Address + Globals::Get()->PosYOffset) / 100.0f),
                         0);
}

uint8_t Instance::GetType() const {
    if (!IsValid()) {
        return 0;
    }
    return Memory::Read<uint8_t>(m_Address + Globals::Get()->InstanceTypeOffset);
}

bool Instance::IsDead() const {
    if (!IsValid()) {
        return true;
    }
    return Memory::Read<bool>(m_Address + Globals::Get()->IsDeadOffset);
}

std::string Instance::GetName() const {
    if (!IsValid()) {
        return "";
    }

    const char* ansiStr = reinterpret_cast<const char*>(m_Address + Globals::Get()->NameOffset);

    wchar_t wideBuffer[65];
    MultiByteToWideChar(1254, 0, ansiStr, -1, wideBuffer, 65);

    char utf8Buffer[195];
    WideCharToMultiByte(CP_UTF8, 0, wideBuffer, -1, utf8Buffer, sizeof(utf8Buffer), NULL, NULL);

    return std::string(utf8Buffer);
}
