#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <winternl.h>

#include <map>

#include "../instance/Instance.h"
#include "../item/Item.h"
#include "../groundItem/GroundItem.h"
#include "Instance.h"
#include "common/Logger.h"
#include "features/Farmbot.h"

typedef std::map<uint32_t, uintptr_t> CharacterInstanceMap;
typedef std::map<uint32_t, uintptr_t> ItemMap;
typedef std::map<uint32_t, uintptr_t> GroundItemMap;

class Helper {
public:
    static bool CompareInstances(const Instance& a, const Instance& b);
    static bool ComparePacketsInstances(const Packets::Instance& a, const Packets::Instance& b);
    static bool CompareGroundItems(const GroundItem& a, const GroundItem& b);
    static Instance GetMainActor();
    static std::vector<Instance> getMobList(MobType targetTypes);
    static std::vector<Packets::Instance> getMobs(MobType targetTypes);
    static void setAttackVid(uint32_t vid);
    static void setAttackState(bool state);
    static uint32_t getTargetVid();

    static void RenderCondition(bool enable);
    static bool ClearRam();
    static std::vector<Math::Vector3>
    DivideTwoPointsByDistance(float distance, Math::Vector3 pointStart, Math::Vector3 pointEnd);
    static void SendAttackPacket(uint32_t vid);
    static void
    SendCharacterStatePacket(Math::Vector3* pos, float rot, uint32_t eFunc, uint32_t uArg);
    static std::vector<Item> GetItemList();
    static GroundItemMap getGroundItemMap();
    static std::vector<GroundItem> getGroundItemList();
    static void SendClickItemPacket(uint32_t vid);

private:
    static CharacterInstanceMap getAlivaInstMap();
    static ItemMap getItemMap();
};