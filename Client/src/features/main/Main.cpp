#include "Main.h"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "core/ClientApp.h"
#include "hack/helper/Helper.h"
#include "hack/instance/Instance.h"
#include "hack/item/Item.h"
#include "hack/groundItem/GroundItem.h"

namespace FracqClient {
Packets::MainState Main::m_settings;
Packets::PickupState Main::m_pickupSettings;
float Main::m_angle = 0.0f;
Math::Vector3 Main::m_mainActorPosLast;

static auto lastWaitHackTime = std::chrono::steady_clock::now();
static auto lastPickupTime = std::chrono::steady_clock::now();
const float Main::safeDistance = 2.0f;
const float Main::radius = (m_settings.AreaSize * 0.6f) + safeDistance;
const float Main::angleStep = (15.0f * 3.14159f) / 180.0f;

void Main::WaitHack() {
    const float distance = 8.0f;
    auto mainActor = InstanceHelper::GetMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mainActorPos = mainActor.GetPixelPosition();
    auto mobList = InstanceHelper::getMobList(m_settings.TargetTypes);

    if (mobList.empty()) {
        return;
    }

    if (mobList.size() > 1) {
        mobList = Helper::FilterByAreaSize(mobList, mainActorPos, distance);
    }

    uint32_t count = 0;

    for (const auto& mob : mobList) {
        auto mobPos = mob.GetPixelPosition();
        float mobDistance = mainActorPos.DistanceTo(mobPos);

        if (count < m_settings.DamageLimit) {
            count++;
            Helper::SendAttackPacket(mob.GetVID());
            Sleep(20);
        }
    }
}

Math::Vector3 Main::FindSafePosition(const Math::Vector3& mainActorPos,
                                     float currentRadius,
                                     float currentAngle,
                                     float currentStep,
                                     const std::vector<Instance>& mobList) {
    auto pos
        = Helper::CalculateCircularMovement(mainActorPos, currentRadius, currentAngle, currentStep);
    if (!Helper::IsMobNearPosition(pos, mobList, 3.0f)) {
        return pos;
    }
    for (int i = 0; i < 3; i++) {
        pos = Helper::CalculateCircularMovement(mainActorPos,
                                                currentRadius,
                                                currentAngle,
                                                currentStep * 1.5f);
        if (!Helper::IsMobNearPosition(pos, mobList, 3.0f)) {
            return pos;
        }
        currentAngle += currentStep * 0.5f;
    }

    float smallerRadius = currentRadius * 0.5f;
    for (int i = 0; i < 3; i++) {
        pos = Helper::CalculateCircularMovement(mainActorPos,
                                                smallerRadius,
                                                currentAngle,
                                                currentStep * 0.5f);
        if (!Helper::IsMobNearPosition(pos, mobList, 3.0f)) {
            return pos;
        }
        currentAngle += currentStep * 0.5f;
    }

    return Helper::CalculateCircularMovement(mainActorPos,
                                             currentRadius,
                                             currentAngle,
                                             currentStep);
}

void Main::RangeDamage() {
    if (m_settings.AreaSize <= 2) {
        return;
    }
    auto mainActor = InstanceHelper::GetMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mainActorPos = mainActor.GetPixelPosition();
    auto mobList = InstanceHelper::getMobList(m_settings.TargetTypes);

    if (mobList.empty()) {
        return;
    }
    mobList = Helper::FilterByAreaSize(mobList, mainActorPos, m_settings.AreaSize);

    uint32_t count = 0;

    for (const auto& mob : mobList) {
        auto mobPos = mob.GetPixelPosition();
        float mobDistance = mainActorPos.DistanceTo(mobPos);

        if (count++ > m_settings.DamageLimit) {
            break;
        }

        Helper::MoveTo(mainActorPos, mobPos);
        Helper::SendAttackPacket(mob.GetVID());

        Helper::MoveTo(mobPos, mainActorPos);

        Sleep(20);
    }
}

void Main::RangeDamageSafe() {
    if (m_settings.AreaSize <= 2) {
        return;
    }
    auto mainActor = InstanceHelper::GetMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mainActorPos = mainActor.GetPixelPosition();
    auto mobList = InstanceHelper::getMobList(m_settings.TargetTypes);

    if (mobList.empty()) {
        return;
    }

    mobList = Helper::FilterByAreaSize(mobList, mainActorPos, m_settings.AreaSize);

    uint32_t count = 0;

    if (m_mainActorPosLast.DistanceTo(mainActorPos) > radius + safeDistance) {
        m_mainActorPosLast = mainActorPos;
        m_angle = 0.0f;
    }

    for (const auto& mob : mobList) {
        auto mobPos = mob.GetPixelPosition();
        float mobDistance = mainActorPos.DistanceTo(mobPos);

        if (count++ > m_settings.DamageLimit) {
            break;
        }

        Helper::MoveTo(mainActorPos, mobPos);
        Helper::SendAttackPacket(mob.GetVID());

        Math::Vector3 nextCircularPos
            = FindSafePosition(mainActorPos, radius, m_angle, angleStep, mobList);
        Helper::MoveTo(mobPos, nextCircularPos);
        m_mainActorPosLast = nextCircularPos;
        m_angle += angleStep;

        Sleep(20);
    }
}

std::vector<GroundItem> Main::FilterGroundItems(const std::vector<GroundItem>& groundItemList,
                                                const Math::Vector3& mainActorPos) {
    std::vector<GroundItem> filtered;

    for (auto& groundItem : groundItemList) {
        auto groundItemPos = groundItem.GetPixelPosition();

        if (mainActorPos.DistanceTo(groundItemPos) >= m_pickupSettings.AreaSize) {
            continue;
        }

        if (!m_pickupSettings.Range && mainActorPos.DistanceTo(groundItemPos) >= 6.0f) {
            continue;
        }

        if (!m_pickupSettings.IncludeAll) {
            bool isItemInList = false;
            uint32_t vnum = groundItem.GetVnum();

            for (size_t i = 0; i < m_pickupSettings.ItemListSize; i++) {
                if (m_pickupSettings.ItemList[i].Vnum == vnum) {
                    isItemInList = true;
                    break;
                }
            }

            if (m_pickupSettings.Include) {
                if (!isItemInList) {
                    continue;
                }
            } else {
                if (isItemInList) {
                    continue;
                }
            }
        }

        filtered.push_back(groundItem);
    }

    return filtered;
}

void Main::PickupGroundItems() {
    auto mainActor = InstanceHelper::GetMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mainActorPos = mainActor.GetPixelPosition();

    auto groundItemList = GroundItemHelper::getGroundItemList();

    if (groundItemList.empty()) {
        return;
    }

    if (m_mainActorPosLast.DistanceTo(mainActorPos) < radius + safeDistance) {
        mainActorPos = m_mainActorPosLast;
    }

    auto filteredGroundItemList = FilterGroundItems(groundItemList, mainActorPos);

    if (filteredGroundItemList.empty()) {
        return;
    }

    Helper::MoveTo(mainActorPos, filteredGroundItemList[0].GetPixelPosition());

    Helper::SendClickItemPacket(filteredGroundItemList[0].GetVID());

    Helper::MoveTo(filteredGroundItemList[0].GetPixelPosition(), mainActorPos);
}

void Main::Loop() {
    if (!s_App) {
        LOG_ERROR(LOG_COMPONENT_FARMBOT, "FarmBot not initialized");
        return;
    }
    m_settings = s_App->GetSettings().Main;

    m_pickupSettings = s_App->GetSettings().Pickup;

    Helper::RenderCondition(m_settings.RenderSkip);

    if (m_settings.ClearRam) {
        Helper::ClearRam();
    }

    auto mainCharacter = InstanceHelper::GetMainActor();
    if (!mainCharacter.IsValid()) {
        return;
    }

    if (mainCharacter.IsDead()) {
        return;
    }

    if (m_settings.DamageEnabled) {

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
            case Packets::DamageType::RangeDamageSafe:
                RangeDamageSafe();
                break;
            default:
                break;
            }
        }
    }

    if (m_pickupSettings.Enabled) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime
                                                                                 - lastPickupTime);

        if (elapsedTime.count() >= m_pickupSettings.Delay) {
            lastPickupTime = currentTime;
            PickupGroundItems();
        }
    }
}

} // namespace FracqClient