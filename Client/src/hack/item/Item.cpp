#include "Item.h"
#include <windows.h>

std::string Item::GetName() const {
    if (m_Address < 0x1000) {
        return "";
    }

    const char* ansiStr = reinterpret_cast<const char*>(m_Address + Globals::Get()->ItemNameOffset);

    wchar_t wideBuffer[65];
    MultiByteToWideChar(1254, 0, ansiStr, -1, wideBuffer, 65);

    char utf8Buffer[195];
    WideCharToMultiByte(CP_UTF8, 0, wideBuffer, -1, utf8Buffer, sizeof(utf8Buffer), NULL, NULL);

    return std::string(utf8Buffer);
}
