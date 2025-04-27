#pragma once
#include "BasePackets.h"
#include "Item.h"

namespace Packets {

struct PickupState {
    size_t ItemListSize;
    std::array<Item, MAX_ITEM_LIST_SIZE> ItemList;
    bool Enabled;
    uint32_t Delay;
    bool Include;
    bool IncludeAll;
    bool Range;
    float AreaSize;

    PickupState()
        : ItemListSize(0)
        , ItemList()
        , Include(false)
        , IncludeAll(true)
        , Range(false)
        , AreaSize(20.0f)
        , Enabled(false)
        , Delay(500) { }
};
} // namespace Packets