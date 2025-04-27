#include "GroundItem.h"
#include <windows.h>
#include "common/Helper.h"

namespace FracqClient {

namespace {
    __declspec(noinline) const char* GetOwnershipInternal(uintptr_t address, size_t offset) {
        const char* ownership = "";
        __try {
            ownership = reinterpret_cast<const char*>(address + offset);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return "";
        }
        return ownership;
    }
}

std::string GroundItem::GetOwnership() const {
    if (!IsValid()) {
        return "";
    }

    const char* ownership = GetOwnershipInternal(m_Address,
                                                 Globals::Get()->GroundItemOwnershipOffset);
    return Common::Helper::ConvertToUTF8(ownership);
}

Math::Vector3 GroundItem::GetPixelPosition() const {
    if (!IsValid()) {
        return Math::Vector3();
    }

    return Math::Vector3(
        abs(Common::Memory::Read<float>(m_Address + Globals::Get()->GroundItemPosXOffset) / 100.0f),
        abs(Common::Memory::Read<float>(m_Address + Globals::Get()->GroundItemPosYOffset) / 100.0f),
        0);
}

uint32_t GroundItem::GetVnum() const {
    if (!IsValid()) {
        return 0;
    }

    return Common::Memory::Read<uint32_t>(m_Address + Globals::Get()->GroundItemVnumOffset);
}

} // namespace FracqClient