#include "FarmBot.h"

#include <algorithm>
#include <chrono>

#include "core/ClientApp.h"
#include "hack/helper/Helper.h"
#include "hack/instance/Instance.h"

namespace FracqClient {

bool FarmBot::m_FixedRange = false;
Math::Vector3 FarmBot::m_FixedRangePos;
Instance LastMob = Instance { 0 };

static auto lastWaitHackTime = std::chrono::steady_clock::now();

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
        float mobDistance = m_FixedRange ? m_FixedRangePos.DistanceTo(mobPos)
                                         : mainActorPos.DistanceTo(mobPos);

        if (mobDistance <= distance) {
            return mob;
        }
    }

    return Instance(0);
}

void FarmBot::WaitHack(MobType targetTypes) {
    const float distance = 8.0f;
    auto mainActor = Helper::GetMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mainActorPos = mainActor.GetPixelPosition();
    auto mobList = Helper::getMobList(targetTypes);

    if (mobList.empty()) {
        return;
    }

    if (mobList.size() > 1) {
        std::sort(mobList.begin(), mobList.end(), &Helper::CompareInstances);
    }

    uint32_t count = 0;

    for (const auto& mob : mobList) {
        auto mobPos = mob.GetPixelPosition();
        float mobDistance = mainActorPos.DistanceTo(mobPos);

        if (mobDistance <= distance) {
            Helper::SendAttackPacket(mob.GetVID());
            Sleep(20);
        }
    }
}

void FarmBot::RangeDamage(MobType targetTypes, float distance) {
    auto mainActor = Helper::GetMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mainActorPos = mainActor.GetPixelPosition();
    auto mobList = Helper::getMobList(targetTypes);

    if (mobList.empty()) {
        return;
    }

    if (mobList.size() > 1) {
        std::sort(mobList.begin(), mobList.end(), &Helper::CompareInstances);
    }

    uint32_t count = 0;

    for (const auto& mob : mobList) {
        auto mobPos = mob.GetPixelPosition();
        float mobDistance = mainActorPos.DistanceTo(mobPos);

        if (mobDistance <= distance && count < 10) {
            count++;

            if (mobDistance >= 6) {
                auto m_points = Helper::DivideTwoPointsByDistance(4, mainActorPos, mobPos);
                for (auto& point : m_points) {
                    Helper::SendCharacterStatePacket(&point, 0, 0, 0);
                    Sleep(3);
                }
            }

            Helper::SendAttackPacket(mob.GetVID());

            if (mobDistance >= 6) {
                auto p_points = Helper::DivideTwoPointsByDistance(4, mobPos, mainActorPos);
                for (auto& point : p_points) {
                    Helper::SendCharacterStatePacket(&point, 0, 0, 0);
                    Sleep(3);
                }
            }
            Sleep(30);
        }
    }
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

    if (settings.WaitHack) {
        /*auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - lastWaitHackTime);

        if (elapsedTime.count() >= 200) {
            RangeDamage(settings.TargetTypes, settings.AreaSize);
            lastWaitHackTime = currentTime;
        }*/
        RangeDamage(settings.TargetTypes, settings.AreaSize);
    }

    if (!settings.FarmBotStatus) {
        if (LastMob.GetAddress()) {
            LastMob = Instance { 0 };
        }
        return;
    }

    if (!Helper::GetMainActor().IsValid()) {
        return;
    }

    if (!LastMob.GetAddress() || LastMob.IsDead()) {
        Instance mob = getAttackableMob(settings.AreaSize, settings.TargetTypes);
        LOG_INFO(LOG_COMPONENT_FARMBOT, "mob vid " << mob.GetVID());
        LOG_INFO(LOG_COMPONENT_FARMBOT, "mob name " << mob.GetName());
        Helper::setAttackVid(mob.GetVID());
        Helper::setAttackState(true);
        LastMob = mob;
    } else {
        Helper::setAttackVid(LastMob.GetVID());
        Helper::setAttackState(true);
    }

    if (settings.AutoLoot) {
        LOG_DEBUG(LOG_COMPONENT_FARMBOT, "Auto-looting enabled, collecting items...");
    }
}

} // namespace FracqClient