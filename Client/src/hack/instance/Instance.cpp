#include "Instance.h"


uint32_t Instance::GetVID() const {
    if(m_Address < 0x1000) { return 0; }
    
    uint32_t vid = Memory::Read<uint32_t>(m_Address + Globals::Get()->VIDOffset);
    return vid > 100 ? vid : 0; 
}

Math::Vector3 Instance::GetPixelPosition() const {
    if (!IsValid()) { return Math::Vector3(); }
    return Math::Vector3(
        abs(Memory::Read<float>(m_Address + Globals::Get()->PosXOffset) / 100.0f),
        abs(Memory::Read<float>(m_Address + Globals::Get()->PosYOffset) / 100.0f),
        0
    );
}

uint8_t Instance::GetType() const {
    if (!IsValid()) { return 0; }
    return Memory::Read<uint8_t>(m_Address + Globals::Get()->InstanceTypeOffset);
}

bool Instance::IsDead() const {
    if (!IsValid()) { return true; }
    return Memory::Read<bool>(m_Address + Globals::Get()->IsDeadOffset);
}

const char* Instance::GetName() const {
    const char* name = "";
    if (!IsValid()) { return name; }

    uintptr_t nameAddress = m_Address + Globals::Get()->NameOffset;
    __asm {
        lea eax, [nameAddress]
        mov eax, [eax]
        mov name, eax
    }
    
    return name;
}

