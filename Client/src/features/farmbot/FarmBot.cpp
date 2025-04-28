#include "FarmBot.h"

#include <algorithm>
#include <chrono>

#include "core/ClientApp.h"
#include "hack/helper/Helper.h"
#include "hack/instance/Instance.h"
#include "hack/gameFunctions/GameFunctions.h"
#include "hack/customGameFunctions/CustomGameFunctions.h"

namespace FracqClient {

bool FarmBot::m_FixedRange = false;
Math::Vector3 FarmBot::m_FixedRangePos;
Instance LastMob = Instance { 0 };

Instance FarmBot::getAttackableMob(float distance, MobType targetTypes) {
    auto mainActor = InstanceHelper::getMainActor();
    if (!mainActor.IsValid()) {
        return Instance(0);
    }

    auto mainActorPos = mainActor.getPixelPosition();
    auto mobList = InstanceHelper::getMobList(targetTypes);

    if (mobList.empty()) {
        return Instance(0);
    }

    if (mobList.size() > 1) {
        std::sort(mobList.begin(), mobList.end(), &Helper::CompareInstances);
    }

    for (const auto& mob : mobList) {
        auto mobPos = mob.getPixelPosition();
        float mobDistance = m_FixedRange ? m_FixedRangePos.DistanceTo(mobPos)
                                         : mainActorPos.DistanceTo(mobPos);

        if (mobDistance <= distance) {
            return mob;
        }
    }

    return Instance(0);
}

void FarmBot::Loop() {
    if (!s_App) {
        LOG_ERROR(LOG_COMPONENT_FARMBOT, "FarmBot not initialized");
        return;
    }

    const auto& settings = s_App->GetSettings().FarmBot;
    Helper::RenderCondition(settings.RenderSkip);

    if (settings.ClearRam) {
        Helper::ClearRam();
    }

    if (!settings.FarmBotStatus) {
        if (LastMob.getAddress()) {
            LastMob = Instance { 0 };
        }
        return;
    }

    if (!InstanceHelper::getMainActor().IsValid()) {
        return;
    }

    if (!LastMob.getAddress() || LastMob.IsDead()) {
        Instance mob = getAttackableMob(settings.AreaSize, settings.TargetTypes);
        LOG_INFO(LOG_COMPONENT_FARMBOT, "mob vid " << mob.getVID());
        LOG_INFO(LOG_COMPONENT_FARMBOT, "mob name " << mob.getName());
        CustomGameFunctions::setAttackVid(mob.getVID());
        CustomGameFunctions::setAttackState(true);
        LastMob = mob;
    } else {
        CustomGameFunctions::setAttackVid(LastMob.getVID());
        CustomGameFunctions::setAttackState(true);
    }

    if (settings.AutoLoot) {
        LOG_DEBUG(LOG_COMPONENT_FARMBOT, "Auto-looting enabled, collecting items...");
    }
}

} // namespace FracqClient