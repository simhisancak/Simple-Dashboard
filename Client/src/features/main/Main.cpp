#include "Main.h"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "core/ClientApp.h"
#include "hack/helper/Helper.h"
#include "hack/instance/Instance.h"
#include "hack/item/Item.h"
#include "hack/groundItem/GroundItem.h"
#include "hack/gameFunctions/GameFunctions.h"
#include "hack/customGameFunctions/CustomGameFunctions.h"
namespace FracqClient {
Packets::MainState Main::m_settings;
Packets::PickupState Main::m_pickupSettings;
float Main::m_angle = 0.0f;
bool Main::m_boost = false;
Math::Vector3 Main::m_mainActorPosLast;

static auto lastDamageTime = std::chrono::steady_clock::now();
static auto lastPickupTime = std::chrono::steady_clock::now();
const float Main::safeDistance = 2.0f;
const float Main::radius = (m_settings.AreaSize * 0.6f) + safeDistance;
const float Main::angleStep = (15.0f * 3.14159f) / 180.0f;

void Main::WaitDamage() {
    const float distance = 8.0f;
    auto mainActor = InstanceHelper::getMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mobList = InstanceHelper::getMobList(m_settings.TargetTypes);

    if (mobList.empty()) {
        return;
    }

    Helper::FilterByAreaSize(mobList, mainActor.getPixelPosition(), distance);

    if (mobList.empty()) {
        return;
    }

    uint32_t count = 0;

    for (const auto& mob : mobList) {
        auto mobPos = mob.getPixelPosition();
        float mobDistance = mainActor.getPixelPosition().DistanceTo(mobPos);

        if (count < m_settings.DamageLimit) {
            count++;
            GameFunctions::SendAttackPacket(mob.getVID());
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
    auto mainActor = InstanceHelper::getMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mobList = InstanceHelper::getMobList(m_settings.TargetTypes);

    if (mobList.empty()) {
        return;
    }
    Helper::FilterByAreaSize(mobList, mainActor.getPixelPosition(), m_settings.AreaSize);

    if (mobList.empty()) {
        return;
    }

    uint32_t count = 0;

    for (const auto& mob : mobList) {
        auto mobPos = mob.getPixelPosition();
        float mobDistance = mainActor.getPixelPosition().DistanceTo(mobPos);

        if (count++ > m_settings.DamageLimit) {
            break;
        }

        CustomGameFunctions::MoveTo(mainActor.getPixelPosition(), mobPos);
        GameFunctions::SendAttackPacket(mob.getVID());

        CustomGameFunctions::MoveTo(mobPos, mainActor.getPixelPosition());
    }
}

void Main::RangeDamageSafe() {
    auto mainActor = InstanceHelper::getMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mobList = InstanceHelper::getMobList(m_settings.TargetTypes);

    if (mobList.empty()) {
        return;
    }

    Helper::FilterByAreaSize(mobList, mainActor.getPixelPosition(), m_settings.AreaSize);

    if (mobList.empty()) {
        return;
    }

    uint32_t count = 0;

    if (m_mainActorPosLast.DistanceTo(mainActor.getPixelPosition()) > radius + safeDistance) {
        m_mainActorPosLast = mainActor.getPixelPosition();
        m_angle = 0.0f;
    }

    for (const auto& mob : mobList) {
        auto mobPos = mob.getPixelPosition();
        float mobDistance = mainActor.getPixelPosition().DistanceTo(mobPos);

        if (count++ > m_settings.DamageLimit) {
            break;
        }

        CustomGameFunctions::MoveTo(m_mainActorPosLast, mobPos);
        GameFunctions::SendAttackPacket(mob.getVID());

        Math::Vector3 nextCircularPos
            = FindSafePosition(mainActor.getPixelPosition(), radius, m_angle, angleStep, mobList);
        CustomGameFunctions::MoveTo(mobPos, nextCircularPos);
        m_mainActorPosLast = nextCircularPos;
        m_angle += angleStep;
    }
}

std::vector<GroundItem> Main::FilterGroundItems(const std::vector<GroundItem>& groundItemList,
                                                const Math::Vector3& mainActorPos) {
    std::vector<GroundItem> filtered;

    for (auto& groundItem : groundItemList) {
        auto groundItemPos = groundItem.getPixelPosition();

        if (mainActorPos.DistanceTo(groundItemPos) >= m_pickupSettings.AreaSize) {
            continue;
        }

        if (!m_pickupSettings.Range && mainActorPos.DistanceTo(groundItemPos) >= 6.0f) {
            continue;
        }

        if (!m_pickupSettings.IncludeAll) {
            bool isItemInList = false;
            uint32_t vnum = groundItem.getVnum();

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
    auto mainActor = InstanceHelper::getMainActor();
    if (!mainActor.IsValid()) {
        return;
    }

    auto mainActorPos = mainActor.getPixelPosition();

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

    CustomGameFunctions::MoveTo(mainActorPos, filteredGroundItemList[0].getPixelPosition());

    GameFunctions::SendClickItemPacket(filteredGroundItemList[0].getVID());

    CustomGameFunctions::MoveTo(filteredGroundItemList[0].getPixelPosition(), mainActorPos);
}

void Main::MoveSpeedHack() {
    auto mainActor = InstanceHelper::getMainActor();

    if (!mainActor.IsValid()) {
        return;
    }

    Math::Vector3 mainActorPos = mainActor.getPixelPosition();

    if (mainActor.getMotionType() != 2) {
        return;
    }

    float rotation = mainActor.getRotation();

    Math::Vector3 newPosition = {
        mainActorPos.x
            + (static_cast<float>(m_settings.MoveSpeed) / 100.0f) * sinf(rotation * 0.017453f),
        mainActorPos.y
            + (static_cast<float>(m_settings.MoveSpeed) / 100.0f) * cosf(rotation * 0.017453f),
        mainActorPos.z
    };

    mainActor.setPixelPosition(newPosition);

    /*if (m_mainActorPosLast.DistanceTo(mainActorPos) < radius + safeDistance) {
        CustomGameFunctions::MoveTo(m_mainActorPosLast, newPosition);
        m_mainActorPosLast = newPosition;
    }*/

    Sleep(1);

    if (m_boost) {
        m_boost = false;
        GameFunctions::SendCharacterStatePacket(newPosition, rotation, 1, 0);
    } else {
        GameFunctions::SendCharacterStatePacket(newPosition, rotation, 0, 0);
        m_boost = true;
    }
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

    auto mainCharacter = InstanceHelper::getMainActor();
    if (!mainCharacter.IsValid()) {
        return;
    }

    if (mainCharacter.IsDead()) {
        return;
    }

    if (m_settings.DamageEnabled) {

        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime
                                                                                 - lastDamageTime);

        if (elapsedTime.count() >= m_settings.DamageDelay) {
            lastDamageTime = currentTime;

            auto start_time = std::chrono::steady_clock::now();

            switch (m_settings.DamageType) {
            case Packets::DamageType::WaitDamage:
                WaitDamage();
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

            auto end_time = std::chrono::steady_clock::now();

            auto _elapsedTime = end_time - start_time;

            LOG_ERROR(LOG_COMPONENT_FARMBOT,
                      "Damage time: " << static_cast<float>(_elapsedTime.count()) / 1000000.0f);
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

    if (m_settings.MoveSpeedEnabled) {
        MoveSpeedHack();
    }
}

} // namespace FracqClient