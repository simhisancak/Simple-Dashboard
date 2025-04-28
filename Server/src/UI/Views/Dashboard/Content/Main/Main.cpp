#include "Main.h"
#include <imgui.h>
#include <windows.h>
#include <algorithm>

#include "PacketTypes.h"
#include "Server/ServerManager.h"
#include "common/Logger.h"

namespace FracqServer {
namespace UI {
    namespace Views {
        const MobTypeUI MOB_TYPES[] = { { "Mobs", MobType::Normal },
                                        { "Metin Stones", MobType::Metin } };

        Main::Main(Application* app)
            : DashboardContent(app) { }

        void Main::Render(int selectedPid) {
            if (selectedPid == -1) {
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - 100.0f,
                                           ImGui::GetWindowHeight() * 0.5f - 10.0f));
                ImGui::Text("Please select a client from the sidebar");
                return;
            }

            ImGui::BeginChild("Controls", ImVec2(ImGui::GetWindowWidth(), 0), false);
            RenderControls(selectedPid);
            ImGui::EndChild();
        }

        void Main::RenderControls(int selectedPid) {
            Server::ClientInfo* client = m_App->GetServerManager()->FindClientByPID(selectedPid);
            if (!client)
                return;

            auto& settings = client->settings.Main;

            const float inputWidth = 60.0f;
            const float labelWidth = 60.0f;
            const float columnWidth = 160.0f;
            const float spacing = 10.0f;
            const float radioStartX = columnWidth;

            {
                ImGui::BeginGroup();

                ImGui::AlignTextToFramePadding();
                ImGui::Checkbox("Enable Damage", &settings.DamageEnabled);

                ImGui::BeginDisabled(!settings.DamageEnabled);

                ImGui::SameLine(radioStartX);
                if (ImGui::RadioButton("wDamage",
                                       settings.DamageType == Packets::DamageType::WaitDamage)) {
                    settings.DamageType = Packets::DamageType::WaitDamage;
                }

                ImGui::SameLine(radioStartX + 120.0f);
                if (ImGui::RadioButton("rDamage",
                                       settings.DamageType == Packets::DamageType::RangeDamage)) {
                    settings.DamageType = Packets::DamageType::RangeDamage;
                }

                ImGui::SameLine(radioStartX + 270.0f);
                if (ImGui::RadioButton("rDamageSafe",
                                       settings.DamageType
                                           == Packets::DamageType::RangeDamageSafe)) {
                    settings.DamageType = Packets::DamageType::RangeDamageSafe;
                }

                ImGui::AlignTextToFramePadding();
                ImGui::SetNextItemWidth(inputWidth);
                int sleepValue = static_cast<int>(settings.DamageDelay);
                if (ImGui::DragInt("Sleep", &sleepValue, 1, 100, 2000)) {
                    sleepValue = std::clamp(sleepValue, 100, 2000);
                    settings.DamageDelay = static_cast<uint32_t>(sleepValue);
                }

                ImGui::SameLine(radioStartX);
                ImGui::SetNextItemWidth(inputWidth);
                float areaValue = settings.AreaSize;
                if (ImGui::DragFloat("Area", &areaValue, 0.5f, 6.0f, 100.0f, "%.1f")) {
                    areaValue = std::clamp(areaValue, 6.0f, 100.0f);
                    settings.AreaSize = std::clamp(areaValue, 6.0f, 100.0f);
                }

                ImGui::SameLine(radioStartX + 120.0f);
                ImGui::SetNextItemWidth(inputWidth);
                int damageLimit = static_cast<int>(settings.DamageLimit);
                if (ImGui::DragInt("Limit", &damageLimit, 1, 1, 20)) {
                    damageLimit = std::clamp(damageLimit, 1, 20);
                    settings.DamageLimit = static_cast<uint32_t>(damageLimit);
                }

                ImGui::AlignTextToFramePadding();
                bool isSelected = (settings.TargetTypes & MOB_TYPES[0].type) == MOB_TYPES[0].type;
                if (ImGui::Checkbox(MOB_TYPES[0].name, &isSelected)) {
                    settings.TargetTypes = isSelected
                                               ? (settings.TargetTypes | MOB_TYPES[0].type)
                                               : static_cast<MobType>(
                                                     static_cast<uint8_t>(settings.TargetTypes)
                                                     & ~static_cast<uint8_t>(MOB_TYPES[0].type));
                }

                ImGui::SameLine(radioStartX);
                isSelected = (settings.TargetTypes & MOB_TYPES[1].type) == MOB_TYPES[1].type;
                if (ImGui::Checkbox(MOB_TYPES[1].name, &isSelected)) {
                    settings.TargetTypes = isSelected
                                               ? (settings.TargetTypes | MOB_TYPES[1].type)
                                               : static_cast<MobType>(
                                                     static_cast<uint8_t>(settings.TargetTypes)
                                                     & ~static_cast<uint8_t>(MOB_TYPES[1].type));
                }

                ImGui::EndDisabled();
                ImGui::EndGroup();
            }

            ImGui::Spacing();
            ImGui::Separator();

            {
                ImGui::BeginGroup();
                ImGui::AlignTextToFramePadding();
                ImGui::Checkbox("Enable Move Speed", &settings.MoveSpeedEnabled);

                ImGui::BeginDisabled(!settings.MoveSpeedEnabled);
                ImGui::SameLine(radioStartX);
                ImGui::SetNextItemWidth(inputWidth);
                int moveSpeedValue = settings.MoveSpeed;
                if (ImGui::DragInt("Speed", &moveSpeedValue, 1, 20, 100)) {
                    moveSpeedValue = std::clamp(moveSpeedValue, 20, 100);
                    settings.MoveSpeed = moveSpeedValue;
                }
                ImGui::EndDisabled();
                ImGui::EndGroup();
            }

            ImGui::Spacing();
            ImGui::Separator();

            {
                ImGui::Checkbox("Render", &settings.RenderSkip);
                ImGui::SameLine(columnWidth);
                ImGui::Checkbox("Ram", &settings.ClearRam);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Minimap");

            const float mapSize = 200.0f;
            const float mapHalfSize = mapSize / 2.0f;
            const float scale = 1.0f;

            ImVec2 mapStart = ImGui::GetCursorScreenPos();
            ImVec2 mapCenter(mapStart.x + mapHalfSize, mapStart.y + mapHalfSize);

            ImGui::GetWindowDrawList()->AddRectFilled(mapStart,
                                                      ImVec2(mapStart.x + mapSize,
                                                             mapStart.y + mapSize),
                                                      IM_COL32(30, 30, 30, 255));

            if (client->memoryInfo.MainActor.VID != 0) {
                ImGui::GetWindowDrawList()->AddCircleFilled(mapCenter,
                                                            5.0f,
                                                            IM_COL32(0, 255, 0, 255) // Yeşil nokta
                );
            }

            const auto& mainPos = client->memoryInfo.MainActor.Position;
            for (size_t i = 0; i < client->memoryInfo.MobListSize; i++) {
                const auto& mob = client->memoryInfo.MobList[i];

                float relativeX = (mob.Position.x - mainPos.x) * scale;
                float relativeY = (mob.Position.y - mainPos.y) * scale;

                if (abs(relativeX) <= mapHalfSize && abs(relativeY) <= mapHalfSize) {
                    ImVec2 mobPos(mapCenter.x + relativeX, mapCenter.y + relativeY);

                    ImU32 mobColor;
                    switch (static_cast<MobType>(mob.Type)) {
                    case MobType::Metin:
                        mobColor = IM_COL32(255, 165, 0, 255); // Turuncu
                        break;
                    case MobType::NPC:
                        mobColor = IM_COL32(0, 255, 255, 255); // Cyan
                        break;
                    case MobType::Player:
                        mobColor = IM_COL32(255, 0, 0, 255); // Kırmızı
                        break;
                    default:
                        mobColor = IM_COL32(255, 255, 255, 255); // Beyaz
                    }

                    ImGui::GetWindowDrawList()->AddCircleFilled(mobPos, 3.0f, mobColor);
                }
            }

            ImGui::GetWindowDrawList()->AddRect(mapStart,
                                                ImVec2(mapStart.x + mapSize, mapStart.y + mapSize),
                                                IM_COL32(255, 255, 255, 255));

            ImGui::Dummy(ImVec2(mapSize, mapSize));

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

    } // namespace Views
} // namespace UI
} // namespace FracqServer
