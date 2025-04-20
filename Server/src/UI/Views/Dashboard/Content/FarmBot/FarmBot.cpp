#include "FarmBot.h"

#include <imgui.h>
#include <windows.h>

#include "PacketTypes.h"
#include "Server/ServerManager.h"
#include "common/Logger.h"

namespace UI {
namespace Views {
struct MobTypeUI {
    const char* name;
    MobType type;
};

const MobTypeUI MOB_TYPES[] = {{"Normal Mobs", MobType::Normal}, {"Metin Stones", MobType::Metin}};

struct handle_data {
    unsigned long process_id;
    HWND window_handle;
};

BOOL is_main_window(HWND handle) {
    return GetWindow(handle, GW_CHILD) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam) {
    handle_data& data = *(handle_data*)lParam;
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);
    if (data.process_id != process_id || !is_main_window(handle)) return TRUE;
    data.window_handle = handle;
    return FALSE;
}

HWND find_main_window(unsigned long process_id) {
    handle_data data;
    data.process_id = process_id;
    data.window_handle = 0;
    EnumWindows(enum_windows_callback, (LPARAM)&data);
    return data.window_handle;
}

FarmBot::FarmBot(Application* app) : DashboardContent(app) {}

void FarmBot::Render(int selectedPid) {
    if (selectedPid == -1) {
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - 100.0f,
                                   ImGui::GetWindowHeight() * 0.5f - 10.0f));
        ImGui::Text("Please select a client from the sidebar");
        return;
    }

    ImGui::BeginChild("Controls", ImVec2(ImGui::GetWindowWidth() * 0.4f, 0), true);
    RenderControls(selectedPid);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Status", ImVec2(0, 0), true);
    RenderStatus(selectedPid);
    ImGui::EndChild();
}

void FarmBot::RenderControls(int selectedPid) {
    Server::ClientInfo* client = m_App->GetServerManager()->FindClientByPID(selectedPid);
    if (!client) return;

    auto& settings = client->settings.FarmBot;

    ImGui::Text("FarmBot Controls");
    ImGui::Separator();

    if (ImGui::Button(settings.FarmBotStatus ? "Stop Bot" : "Start Bot", ImVec2(150, 30))) {
        settings.FarmBotStatus = !settings.FarmBotStatus;
    }

    if (ImGui::Button(settings.WaitHack ? "Stop Wait Hack" : "Start WaitHack", ImVec2(150, 30))) {
        settings.WaitHack = !settings.WaitHack;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Bot Settings");

    ImGui::Checkbox("Auto Loot", &settings.AutoLoot);

    ImGui::Checkbox("Render Skip", &settings.RenderSkip);

    ImGui::Checkbox("Clear Ram", &settings.ClearRam);

    ImGui::SliderFloat("Area Size", &settings.AreaSize, 10.0f, 100.0f, "%.1f");

    ImGui::Spacing();
    ImGui::Text("Target Types");

    for (const auto& mob : MOB_TYPES) {
        bool isSelected = (settings.TargetTypes & mob.type) == mob.type;
        if (ImGui::Checkbox(mob.name, &isSelected)) {
            settings.TargetTypes = isSelected ? (settings.TargetTypes | mob.type)
                                              : static_cast<MobType>(
                                                    static_cast<uint8_t>(settings.TargetTypes)
                                                    & ~static_cast<uint8_t>(mob.type));
        }
    }
}

void FarmBot::RenderStatus(int selectedPid) {
    Server::ClientInfo* client = m_App->GetServerManager()->FindClientByPID(selectedPid);
    if (!client) return;

    auto& settings = client->settings.FarmBot;

    ImGui::Text("Bot Status");
    ImGui::Separator();

    ImGui::Text("Client PID: %d", selectedPid);
    ImGui::Text("Status: %s", settings.FarmBotStatus ? "Running" : "Stopped");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Current Settings");
    ImGui::Text("Auto Loot: %s", settings.AutoLoot ? "Enabled" : "Disabled");
    ImGui::Text("Wait Hack: %s", settings.WaitHack ? "Enabled" : "Disabled");
    ImGui::Text("Render Skip: %s", settings.RenderSkip ? "Enabled" : "Disabled");
    ImGui::Text("Clear Ram: %s", settings.ClearRam ? "Enabled" : "Disabled");
    ImGui::Text("Area Size: %.1f", settings.AreaSize);
    ImGui::Text("Target Types: ");
    ImGui::SameLine();

    bool first = true;
    for (const auto& mob : MOB_TYPES) {
        if ((settings.TargetTypes & mob.type) == mob.type) {
            if (!first) ImGui::SameLine();
            ImGui::Text("%s", mob.name);
            first = false;
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Memory State");
    ImGui::Text("Position: X=%.1f Y=%.1f Z=%.1f",
                client->memoryInfo.MainActor.Position.x,
                client->memoryInfo.MainActor.Position.y,
                client->memoryInfo.MainActor.Position.z);
    ImGui::Text("Mob List Size: %d", client->memoryInfo.MobListSize);

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
                                              ImVec2(mapStart.x + mapSize, mapStart.y + mapSize),
                                              IM_COL32(30, 30, 30, 255));

    if (client->memoryInfo.MainActor.VID != 0) {
        ImGui::GetWindowDrawList()->AddCircleFilled(mapCenter,
                                                    5.0f,
                                                    IM_COL32(0, 255, 0, 255)  // Yeşil nokta
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
                    mobColor = IM_COL32(255, 165, 0, 255);  // Turuncu
                    break;
                case MobType::NPC:
                    mobColor = IM_COL32(0, 255, 255, 255);  // Cyan
                    break;
                case MobType::Player:
                    mobColor = IM_COL32(255, 0, 0, 255);  // Kırmızı
                    break;
                default:
                    mobColor = IM_COL32(255, 255, 255, 255);  // Beyaz
            }

            ImGui::GetWindowDrawList()->AddCircleFilled(mobPos, 3.0f, mobColor);
        }
    }

    ImGui::GetWindowDrawList()->AddRect(mapStart,
                                        ImVec2(mapStart.x + mapSize, mapStart.y + mapSize),
                                        IM_COL32(255, 255, 255, 255));

    ImGui::Dummy(ImVec2(mapSize, mapSize));
}
}  // namespace Views
}  // namespace UI