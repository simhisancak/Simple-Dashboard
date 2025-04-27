#include "Helper.h"
#include <algorithm>

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
} // namespace Common