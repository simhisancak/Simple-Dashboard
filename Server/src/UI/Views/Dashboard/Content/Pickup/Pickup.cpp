#include "Pickup.h"
#include <imgui.h>
#include <windows.h>
#include <algorithm>
#include <string>

#include "PacketTypes.h"
#include "Server/ServerManager.h"
#include "common/Logger.h"
#include "common/Helper.h"

namespace FracqServer {
namespace UI {
    namespace Views {
        Pickup::Pickup(Application* app)
            : DashboardContent(app) { }

        void Pickup::Render(int selectedPid) {
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

        void Pickup::RenderControls(int selectedPid) {
            Server::ClientInfo* client = m_App->GetServerManager()->FindClientByPID(selectedPid);
            if (!client)
                return;

            auto& settings = client->settings.Pickup;

            const float inputWidth = 60.0f;
            const float labelWidth = 60.0f;
            const float columnWidth = 160.0f;
            const float spacing = 10.0f;

            ImVec2 contentSize = ImGui::GetContentRegionAvail();
            float upperSectionHeight = 70.0f;
            float lowerSectionHeight = ImGui::GetWindowHeight() - upperSectionHeight - spacing * 2;
            float totalWidth = contentSize.x;

            ImGui::BeginGroup();
            {
                ImGui::BeginChild("UpperControls", ImVec2(totalWidth, upperSectionHeight), false);
                {
                    float checkboxWidth = totalWidth / 3;

                    // Enabled checkbox
                    ImGui::Checkbox("Enabled", &settings.Enabled);

                    // Begin disabled section for all controls if Enabled is false
                    ImGui::BeginDisabled(!settings.Enabled);

                    // Include All checkbox
                    ImGui::SameLine(checkboxWidth);
                    ImGui::Checkbox("Include All", &settings.IncludeAll);

                    // Radio buttons for Include/Not Include (disabled if Include All is checked)
                    ImGui::BeginDisabled(settings.IncludeAll);
                    ImGui::SameLine(checkboxWidth * 2);
                    if (ImGui::RadioButton("Include", settings.Include)) {
                        settings.Include = true;
                    }
                    ImGui::SameLine();
                    if (ImGui::RadioButton("Not Include", !settings.Include)) {
                        settings.Include = false;
                    }
                    ImGui::EndDisabled();

                    // Range controls
                    ImGui::Checkbox("Range", &settings.Range);
                    ImGui::SameLine(checkboxWidth);
                    ImGui::SetNextItemWidth(100.0f);
                    float areaValue = settings.AreaSize;
                    if (ImGui::DragFloat("Area Size", &areaValue, 0.1f, 6.0f, 100.0f, "%.1f")) {
                        areaValue = std::clamp(areaValue, 6.0f, 100.0f);
                        settings.AreaSize = std::clamp(areaValue, 6.0f, 100.0f);
                    }

                    ImGui::SameLine(checkboxWidth * 2);
                    ImGui::SetNextItemWidth(100.0f);
                    int sleepValue = static_cast<int>(settings.Delay);
                    if (ImGui::DragInt("Sleep", &sleepValue, 1, 100, 2000)) {
                        sleepValue = std::clamp(sleepValue, 100, 2000);
                        settings.Delay = static_cast<uint32_t>(sleepValue);
                    }

                    // End disabled section for all controls
                    ImGui::EndDisabled();
                }
                ImGui::EndChild();

                ImGui::Separator();

                ImGui::BeginChild("LowerControls", ImVec2(totalWidth, lowerSectionHeight), false);
                {
                    float listBoxWidth = (totalWidth - spacing * 3) / 2.0f;
                    float leftPadding = spacing;

                    ImGui::SetCursorPosX(leftPadding);
                    ImGui::BeginGroup();
                    ImGui::Text("Item List");

                    ImGui::SetNextItemWidth(listBoxWidth);
                    if (ImGui::InputText("##Filter", m_FilterBuffer, Packets::MAX_NAME_LENGTH)) {
                        size_t len = strlen(m_FilterBuffer);
                        if (len >= 3) {
                            m_App->GetServerManager()->RequestItemDump(selectedPid, m_FilterBuffer);
                        } else {
                            m_App->GetServerManager()->ClearItemDumpState(selectedPid);
                        }
                    }

                    float remainingHeight = lowerSectionHeight
                                            - ImGui::GetTextLineHeightWithSpacing() * 2
                                            - spacing * 2 - 8.0f;

                    if (ImGui::BeginListBox("##ItemDump", ImVec2(listBoxWidth, remainingHeight))) {
                        for (size_t i = 0; i < client->itemDumpState.ItemListSize; i++) {
                            const char* itemName = client->itemDumpState.ItemList[i].Name;
                            uint32_t itemVnum = client->itemDumpState.ItemList[i].Vnum;
                            bool alreadySelected = false;

                            for (size_t j = 0; j < client->settings.Pickup.ItemListSize; j++) {
                                if (client->settings.Pickup.ItemList[j].Vnum == itemVnum) {
                                    alreadySelected = true;
                                    break;
                                }
                            }

                            if (!alreadySelected) {
                                ImGui::PushID(static_cast<int>(itemVnum));
                                char displayName[256];
                                snprintf(displayName,
                                         sizeof(displayName),
                                         "[%u] %s",
                                         itemVnum,
                                         itemName);
                                bool clicked = ImGui::Selectable(displayName, false);
                                bool hovered = ImGui::IsItemHovered();
                                bool doubleClicked = ImGui::IsMouseDoubleClicked(
                                    ImGuiMouseButton_Left);

                                if (hovered && doubleClicked) {
                                    if (client->settings.Pickup.ItemListSize
                                        < Packets::MAX_ITEM_LIST_SIZE) {
                                        size_t newIndex = client->settings.Pickup.ItemListSize;
                                        strncpy(client->settings.Pickup.ItemList[newIndex].Name,
                                                itemName,
                                                Packets::MAX_NAME_LENGTH - 1);
                                        client->settings.Pickup.ItemList[newIndex]
                                            .Name[Packets::MAX_NAME_LENGTH - 1]
                                            = '\0';
                                        client->settings.Pickup.ItemList[newIndex].Vnum = itemVnum;
                                        client->settings.Pickup.ItemListSize++;
                                    }
                                }
                                ImGui::PopID();
                            }
                        }
                        ImGui::EndListBox();
                    }
                    ImGui::EndGroup();

                    ImGui::SameLine(leftPadding + listBoxWidth + spacing);
                    ImGui::BeginGroup();
                    ImGui::Text("Selected Items");

                    ImGui::SetNextItemWidth(listBoxWidth);
                    static char selectedItemsFilter[Packets::MAX_NAME_LENGTH] = "";
                    ImGui::InputText("##SelectedFilter",
                                     selectedItemsFilter,
                                     Packets::MAX_NAME_LENGTH);

                    if (ImGui::BeginListBox("##SelectedItems",
                                            ImVec2(listBoxWidth, remainingHeight))) {
                        for (size_t i = 0; i < client->settings.Pickup.ItemListSize; i++) {
                            const char* itemName = client->settings.Pickup.ItemList[i].Name;
                            uint32_t itemVnum = client->settings.Pickup.ItemList[i].Vnum;

                            // Filter check
                            if (strlen(selectedItemsFilter) > 0) {
                                char displayName[256];
                                snprintf(displayName,
                                         sizeof(displayName),
                                         "[%u] %s",
                                         itemVnum,
                                         itemName);
                                if (!Common::Helper::ContainsCaseInsensitive(displayName,
                                                                             selectedItemsFilter)) {
                                    continue;
                                }
                            }

                            ImGui::PushID(static_cast<int>(itemVnum));
                            char displayName[256];
                            snprintf(displayName,
                                     sizeof(displayName),
                                     "[%u] %s",
                                     itemVnum,
                                     itemName);
                            bool clicked = ImGui::Selectable(displayName, false);
                            bool hovered = ImGui::IsItemHovered();
                            bool doubleClicked = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

                            if (hovered && doubleClicked) {
                                for (size_t j = i; j < client->settings.Pickup.ItemListSize - 1;
                                     j++) {
                                    strncpy(client->settings.Pickup.ItemList[j].Name,
                                            client->settings.Pickup.ItemList[j + 1].Name,
                                            Packets::MAX_NAME_LENGTH);
                                    client->settings.Pickup.ItemList[j].Vnum
                                        = client->settings.Pickup.ItemList[j + 1].Vnum;
                                }
                                client->settings.Pickup.ItemListSize--;
                            }
                            ImGui::PopID();
                        }
                        ImGui::EndListBox();
                    }
                    ImGui::EndGroup();
                }
                ImGui::EndChild();
            }
            ImGui::EndGroup();
        }

    } // namespace Views
} // namespace UI
} // namespace FracqServer
