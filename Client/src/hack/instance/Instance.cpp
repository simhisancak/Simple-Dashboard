#include "Instance.h"
#include <windows.h>
#include "common/Helper.h"

namespace FracqClient {

namespace {
    __declspec(noinline) const char* GetNameFromAsm(uintptr_t address, uintptr_t getnamecall) {
        const char* name = "";
        __asm {
            mov ecx, address
            call getnamecall
            mov name, eax
        }
        return name;
    }

    const char* GetNameInternal(uintptr_t address, uintptr_t getnamecall) {
        const char* name = "";
        try {
            name = GetNameFromAsm(address, getnamecall);
        } catch (...) {
            return "";
        }
        return name;
    }
}

uint32_t Instance::GetVID() const {
    if (m_Address < 0x1000) {
        return 0;
    }

    uint32_t vid = Common::Memory::Read<uint32_t>(m_Address + Globals::Get()->VIDOffset);
    return vid > 100 ? vid : 0;
}

Math::Vector3 Instance::GetPixelPosition() const {
    if (!IsValid()) {
        return Math::Vector3();
    }
    return Math::Vector3(
        abs(Common::Memory::Read<float>(m_Address + Globals::Get()->PosXOffset) / 100.0f),
        abs(Common::Memory::Read<float>(m_Address + Globals::Get()->PosYOffset) / 100.0f),
        0);
}

uint8_t Instance::GetType() const {
    if (!IsValid()) {
        return 0;
    }
    return Common::Memory::Read<uint8_t>(m_Address + Globals::Get()->InstanceTypeOffset);
}

bool Instance::IsDead() const {
    if (!IsValid()) {
        return true;
    }
    return Common::Memory::Read<bool>(m_Address + Globals::Get()->IsDeadOffset);
}

std::string Instance::GetName() const {
    if (!IsValid()) {
        return "";
    }

    uintptr_t getnamecall = Globals::Get()->GetNameString;
    const char* name = GetNameInternal(m_Address, getnamecall);
    return Common::Helper::ConvertToUTF8(name);
}

} // namespace FracqClient
