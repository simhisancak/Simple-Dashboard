#include "FarmBot.h"
#include "core/ClientApp.h"
#include "hack/instance/Instance.h"
#include "hack/helper/Helper.h"
#include <algorithm>


bool FarmBot::m_FixedRange = false;
Math::Vector3 FarmBot::m_FixedRangePos;

Instance FarmBot::getAttackableMob(float distance, MobType targetTypes) {
    auto mainActor = Helper::GetMainActor();
    if (!mainActor.IsValid()) {
        return Instance(0);
    }

    auto mainActorPos = mainActor.GetPixelPosition();
    auto mobList = Helper::getMobList(targetTypes);
    
    if (mobList.empty()) {
        return Instance(0);
    }
    
    if (mobList.size() > 1) {
        std::sort(mobList.begin(), mobList.end(), &Helper::CompareInstances);
    }  

    for (const auto& mob : mobList) {
        auto mobPos = mob.GetPixelPosition();
        float mobDistance = m_FixedRange ? 
            m_FixedRangePos.DistanceTo(mobPos) : 
            mainActorPos.DistanceTo(mobPos);

        if (mobDistance <= distance) {
            return mob;
        }
    }
    
    return Instance(0);
}

Instance LastMob = Instance{0};

void FarmBot::Loop() {
    if (!s_App) {
        LOG_ERROR(LOG_COMPONENT_FARMBOT, "FarmBot not initialized");
        return;
    }

    const auto& settings = s_App->GetSettings().FarmBot;
    Helper::RenderCondition(settings.RenderSkip);

    if (settings.ClearRam)
    {
        Helper::ClearRam();
    }

    if (!settings.FarmBotStatus) {
        if(LastMob.GetAddress()){ LastMob = Instance{0}; }
        return;
    }

    if(!Helper::GetMainActor().IsValid()){
        return;
    }

    if(!LastMob.GetAddress() || LastMob.IsDead()){
        Instance mob = getAttackableMob(settings.AreaSize, settings.TargetTypes);
        LOG_INFO(LOG_COMPONENT_FARMBOT, "mob vid " << mob.GetVID());
        LOG_INFO(LOG_COMPONENT_FARMBOT, "mob name " << mob.GetName());
        Helper::setTargetVid(mob.GetVID());
        LastMob = mob;
    }
    else {
        Helper::setTargetVid(LastMob.GetVID());
    }

    if (settings.AutoLoot) {
        LOG_DEBUG(LOG_COMPONENT_FARMBOT, "Auto-looting enabled, collecting items...");
    }
}