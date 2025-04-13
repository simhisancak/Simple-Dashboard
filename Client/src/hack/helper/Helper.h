#pragma once
#include "features/Farmbot.h"
#include "../instance/Instance.h"
#include "common/Logger.h"
#include "Instance.h"
#include <map>
#include <Windows.h>

using TCharacterInstanceMap = std::map<uint32_t, uintptr_t>;

class Helper
{
public:
    static bool CompareInstances(const Instance &a, const Instance &b);
    static bool ComparePacketsInstances(const Packets::Instance &a, const Packets::Instance &b);
    static Instance GetMainActor();
    static std::vector<Instance> getMobList(MobType targetTypes);
    static std::vector<Packets::Instance> getMobs(MobType targetTypes);
    static void setTargetVid(uint32_t vid);
    static TCharacterInstanceMap getAlivaInstMap();
    static void RenderCondition(bool enable);
};