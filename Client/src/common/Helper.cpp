#include "Helper.h"
#include <algorithm>
#include <windows.h>

namespace Common {

bool Helper::ContainsCaseInsensitive(const std::string& content, const std::string& searchText) {
    if (searchText.empty())
        return true;

    std::string lowerContent = content;
    std::string lowerSearchText = searchText;

    std::transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(), ::tolower);
    std::transform(lowerSearchText.begin(),
                   lowerSearchText.end(),
                   lowerSearchText.begin(),
                   ::tolower);

    return lowerContent.find(lowerSearchText) != std::string::npos;
}

std::string Helper::ConvertToUTF8(const char* text) {
    if (!text)
        return "";

    wchar_t wideBuffer[65];
    MultiByteToWideChar(1254, 0, text, -1, wideBuffer, 65);

    char utf8Buffer[195];
    WideCharToMultiByte(CP_UTF8, 0, wideBuffer, -1, utf8Buffer, sizeof(utf8Buffer), NULL, NULL);

    return std::string(utf8Buffer);
}

} // namespace Common