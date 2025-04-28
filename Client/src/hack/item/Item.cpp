#include "Item.h"
#include <windows.h>
#include "common/Helper.h"

namespace FracqClient {

namespace {
    __declspec(noinline) const char* getNameInternal(uintptr_t address, size_t offset) {
        const char* name = "";
        __try {
            name = reinterpret_cast<const char*>(address + offset);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return "";
        }
        return name;
    }
}

std::string Item::getName() const {
    if (m_Address < 0x10000) {
        return "";
    }
    const char* name = getNameInternal(m_Address, Globals::Get()->ItemNameOffset);
    return Common::Helper::ConvertToUTF8(name);
}

uint32_t Item::getVnum() const {
    if (!IsValid()) {
        return 0;
    }
    return Common::Memory::Read<uint32_t>(m_Address + Globals::Get()->ItemVnumOffset);
}

} // namespace FracqClient
