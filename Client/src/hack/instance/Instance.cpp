#include "Instance.h"
#include <windows.h>
#include "common/Helper.h"

namespace FracqClient {

namespace {
    __declspec(noinline) const char* getNameFromAsm(uintptr_t address, uintptr_t getnamecall) {
        const char* name = "";
        __asm {
            mov ecx, address
            call getnamecall
            mov name, eax
        }
        return name;
    }

    const char* getNameInternal(uintptr_t address, uintptr_t getnamecall) {
        const char* name = "";
        try {
            name = getNameFromAsm(address, getnamecall);
        } catch (...) {
            return "";
        }
        return name;
    }
}

uint32_t Instance::getVID() const {
    if (m_Address < 0x10000) {
        return 0;
    }

    uint32_t vid = Common::Memory::Read<uint32_t>(m_Address + Globals::Get()->VIDOffset);
    return vid > 100 ? vid : 0;
}

Math::Vector3 Instance::getPixelPosition() const {
    if (!IsValid()) {
        return Math::Vector3();
    }
    return Math::Vector3(
        abs(Common::Memory::Read<float>(m_Address + Globals::Get()->PosXOffset) / 100.0f),
        abs(Common::Memory::Read<float>(m_Address + Globals::Get()->PosYOffset) / 100.0f),
        0);
}
void Instance::setPixelPosition(Math::Vector3 position) const {
    if (!IsValid()) {
        return;
    }
    Common::Memory::Write<float>(m_Address + Globals::Get()->PosXOffset, position.x * 100.0f);
    Common::Memory::Write<float>(m_Address + Globals::Get()->PosYOffset,
                                 -1 * (position.y * 100.0f));
}

uint8_t Instance::getType() const {
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

std::string Instance::getName() const {
    if (!IsValid()) {
        return "";
    }

    uintptr_t getnamecall = Globals::Get()->GetNameString;
    const char* name = getNameInternal(m_Address, getnamecall);
    return Common::Helper::ConvertToUTF8(name);
}

uint32_t Instance::getRaceMotionData() const {
    if (!IsValid()) {
        return 0;
    }
    return Common::Memory::Read<uint32_t>(m_Address + Globals::Get()->RaceMotionDataPtrOffset);
}

uint32_t Instance::getMotionType() const {
    if (!IsValid()) {
        return 0;
    }
    return Common::Memory::Read<uint32_t>(getRaceMotionData() + 0x4);
}

float Instance::getRotation() const {
    if (!IsValid()) {
        return 0.0f;
    }
    return Common::Memory::Read<float>(m_Address + Globals::Get()->RotationOffset);
}

} // namespace FracqClient
