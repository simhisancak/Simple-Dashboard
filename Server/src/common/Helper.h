#pragma once
#include <string>

namespace Common {

class Helper {
public:
    static bool ContainsCaseInsensitive(const std::string& content, const std::string& searchText);
};
} // namespace Common