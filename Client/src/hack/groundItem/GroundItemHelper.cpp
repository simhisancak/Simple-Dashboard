#include "GroundItem.h"
#include "hack/instance/Instance.h"

namespace FracqClient {

namespace {
    __declspec(noinline) bool ReadGroundItemMap(uintptr_t map_address, GroundItemMap& out_map) {
        __try {
            out_map = *reinterpret_cast<GroundItemMap*>(map_address);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }
}

GroundItemMap GroundItemHelper::getGroundItemMap() {
    uintptr_t m_GroundItemMap_p = Common::Memory::Read<uintptr_t>(
        Globals::Get()->PythonItem + Globals::Get()->GroundItemMapOffset);
    if (m_GroundItemMap_p < 0x10000) {
        return GroundItemMap();
    }

    uintptr_t m_GroundItemMap = Common::Memory::Read<uintptr_t>(m_GroundItemMap_p + 0x4);
    if (m_GroundItemMap < 0x10000) {
        return GroundItemMap();
    }

    GroundItemMap result;
    return ReadGroundItemMap(m_GroundItemMap, result) ? result : GroundItemMap();
}

std::vector<GroundItem> GroundItemHelper::getGroundItemList() {
    auto mainActor = InstanceHelper::getMainActor();

    GroundItemMap groundItemMap = getGroundItemMap();
    std::vector<GroundItem> groundItemList;

    for (const auto& item : groundItemMap) {
        GroundItem groundItem = GroundItem::FromAddress(item.second, item.first);
        if (!groundItem.IsValid())
            continue;

        std::string ownership = groundItem.getOwnership();
        std::string playerName = mainActor.getName();

        if (!ownership.empty() && ownership.compare(playerName) != 0) {
            continue;
        }

        groundItemList.push_back(groundItem);
    }

    return groundItemList;
}

} // namespace FracqClient