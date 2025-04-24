#include "Main.h"
#include <imgui.h>
#include <windows.h>

#include "PacketTypes.h"
#include "Server/ServerManager.h"
#include "common/Logger.h"

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

        // Damage Controls Group
        {
            ImGui::BeginGroup();

            // First row - Enable and types
            ImGui::AlignTextToFramePadding();
            ImGui::Checkbox("Enable Hack", &settings.DamageEnabled);
            ImGui::SameLine(columnWidth - 50.0f);
            if (ImGui::RadioButton("WaitHack",
                                   settings.DamageType == Packets::DamageType::WaitHack)) {
                settings.DamageType = Packets::DamageType::WaitHack;
            }
            ImGui::SameLine(columnWidth + 80.0f);
            if (ImGui::RadioButton("Range Damage",
                                   settings.DamageType == Packets::DamageType::RangeDamage)) {
                settings.DamageType = Packets::DamageType::RangeDamage;
            }

            if (!settings.DamageEnabled) {
                settings.DamageType = Packets::DamageType::None;
            }

            // Second row - Sleep, Count, Area and Limit
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Sleep");
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(inputWidth);
            int sleepValue = static_cast<int>(settings.DamageDelay);
            if (ImGui::InputInt("##DamageSleep",
                                &sleepValue,
                                0,
                                0,
                                ImGuiInputTextFlags_CharsDecimal)) {
                settings.DamageDelay = static_cast<uint32_t>(
                    std::max(10, std::min(sleepValue, 1000)));
            }

            ImGui::SameLine(labelWidth + inputWidth + spacing * 2);
            ImGui::Text("Count");
            ImGui::SameLine(labelWidth * 2 + inputWidth + spacing * 2);
            ImGui::SetNextItemWidth(inputWidth);
            int countValue = static_cast<int>(settings.DamageCount);
            if (ImGui::InputInt("##DamageCount",
                                &countValue,
                                0,
                                0,
                                ImGuiInputTextFlags_CharsDecimal)) {
                settings.DamageCount = static_cast<uint32_t>(std::max(1, std::min(countValue, 50)));
            }

            ImGui::SameLine(labelWidth * 2 + inputWidth * 2 + spacing * 4);
            ImGui::Text("Area");
            ImGui::SameLine(labelWidth * 3 + inputWidth * 2 + spacing * 4);
            ImGui::SetNextItemWidth(inputWidth);
            float areaSize = settings.AreaSize;
            if (ImGui::InputFloat("##AreaSize", &areaSize, 0.0f, 0.0f, "%.1f")) {
                settings.AreaSize = std::max(0.0f, std::min(areaSize, 100.0f));
            }

            ImGui::SameLine(labelWidth * 3 + inputWidth * 3 + spacing * 6);
            ImGui::Text("Limit");
            ImGui::SameLine(labelWidth * 4 + inputWidth * 3 + spacing * 6);
            ImGui::SetNextItemWidth(inputWidth);
            int damageLimit = static_cast<int>(settings.DamageLimit);
            if (ImGui::InputInt("##DamageLimit",
                                &damageLimit,
                                0,
                                0,
                                ImGuiInputTextFlags_CharsDecimal)) {
                settings.DamageLimit = static_cast<uint32_t>(
                    std::max(1, std::min(damageLimit, 20)));
            }

            // Third row - Target Types
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Target");
            ImGui::SameLine(labelWidth);
            for (int i = 0; i < sizeof(MOB_TYPES) / sizeof(MOB_TYPES[0]); i++) {
                if (i > 0)
                    ImGui::SameLine(columnWidth);
                bool isSelected = (settings.TargetTypes & MOB_TYPES[i].type) == MOB_TYPES[i].type;
                if (ImGui::Checkbox(MOB_TYPES[i].name, &isSelected)) {
                    settings.TargetTypes = isSelected
                                               ? (settings.TargetTypes | MOB_TYPES[i].type)
                                               : static_cast<MobType>(
                                                     static_cast<uint8_t>(settings.TargetTypes)
                                                     & ~static_cast<uint8_t>(MOB_TYPES[i].type));
                }
            }
            ImGui::EndGroup();
        }

        ImGui::Spacing();
        ImGui::Separator();

        // Bot Settings row
        {
            ImGui::Text("Optimize");
            ImGui::SameLine(labelWidth);
            ImGui::Checkbox("Render", &settings.RenderSkip);
            ImGui::SameLine(columnWidth + 80.0f);
            ImGui::Checkbox("Ram", &settings.ClearRam);
            ImGui::SameLine(columnWidth * 2 + 80.0f);
            ImGui::Checkbox("Pickup", &settings.Pickup);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Minimap
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
    }
} // namespace Views
} // namespace UI