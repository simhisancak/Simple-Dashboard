#include "Main.h"

#include <algorithm>
#include <chrono>

#include "core/ClientApp.h"
#include "hack/helper/Helper.h"
#include "hack/instance/Instance.h"
#include "hack/item/Item.h"

Packets::MainState Main::m_settings;

// Add static variable for timing
static auto lastWaitHackTime = std::chrono::steady_clock::now();

void Main::WaitHack() {
    const float distance = 8.0f;
    auto mainActor = Helper::GetMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mainActorPos = mainActor.GetPixelPosition();
    auto mobList = Helper::getMobList(m_settings.TargetTypes);

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

        if (mobDistance <= distance && count < m_settings.DamageCount) {
            count++;
            Helper::SendAttackPacket(mob.GetVID());
            Sleep(20);
        }
    }
}

void Main::RangeDamage() {
    auto mainActor = Helper::GetMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mainActorPos = mainActor.GetPixelPosition();
    auto mobList = Helper::getMobList(m_settings.TargetTypes);

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

        if (mobDistance <= m_settings.AreaSize && count < m_settings.DamageCount) {
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

void Main::Loop() {
    if (!s_App) {
        LOG_ERROR(LOG_COMPONENT_FARMBOT, "FarmBot not initialized");
        return;
    }
    m_settings = s_App->GetSettings().Main;
    Helper::RenderCondition(m_settings.RenderSkip);

    if (m_settings.ClearRam) {
        Helper::ClearRam();
    }

    if (m_settings.DamageEnabled) {

        TItemMap m_ItemMap = *(TItemMap*)(*reinterpret_cast<DWORD*>(
            *reinterpret_cast<DWORD*>(Globals::Get()->ItemManager + Globals::Get()->ItemMapOffset)
            + 4));

        LOG_ERROR(LOG_COMPONENT_FARMBOT, "ItemMap: " << m_ItemMap.size());

        for (const auto& item : m_ItemMap) {
            auto _item = Item::FromAddress(item.second);

            if (!_item.IsValid()) {
                continue;
            }
            LOG_ERROR(LOG_COMPONENT_FARMBOT, "Item: " << item.first << " " << _item.GetName());
        }

        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - lastWaitHackTime);

        if (elapsedTime.count() >= m_settings.DamageDelay) {
            lastWaitHackTime = currentTime;

            switch (m_settings.DamageType) {
            case Packets::DamageType::WaitHack:
                WaitHack();
                break;
            case Packets::DamageType::RangeDamage:
                RangeDamage();
                break;
            default:
                break;
            }
        }
    }
}