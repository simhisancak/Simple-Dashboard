#pragma once
#include <string>

namespace Common {

class Helper {
public:
    static bool ContainsCaseInsensitive(const std::string& content, const std::string& searchText);
    static std::string ConvertToUTF8(const char* text);
};
} // namespace Common