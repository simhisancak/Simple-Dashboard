#include "Item.h"

namespace FracqClient {

namespace {
    __declspec(noinline) bool ReadItemMap(uintptr_t map_address, ItemMap& out_map) {
        __try {
            out_map = *reinterpret_cast<ItemMap*>(map_address);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }
}

ItemMap ItemHelper::getItemMap() {
    uintptr_t m_ItemMap_p = Common::Memory::Read<uintptr_t>(Globals::Get()->ItemManager
                                                            + Globals::Get()->ItemMapOffset);
    if (m_ItemMap_p < 0x10000) {
        return ItemMap();
    }

    uintptr_t m_ItemMap = Common::Memory::Read<uintptr_t>(m_ItemMap_p + 0x4);
    if (m_ItemMap < 0x10000) {
        return ItemMap();
    }

    ItemMap result;
    return ReadItemMap(m_ItemMap, result) ? result : ItemMap();
}

std::vector<Item> ItemHelper::getItemList() {
    ItemMap itemMap = getItemMap();
    std::vector<Item> itemList;

    for (const auto& Itm : itemMap) {
        Item item = Item::FromAddress(Itm.second);
        if (!item.IsValid())
            continue;

        itemList.push_back(item);
    }

    return itemList;
}

} // namespace FracqClient