#include "SaveLoad.h"
#include <imgui.h>

namespace UI {
namespace Views {

SaveLoad::SaveLoad(Application* app)
    : DashboardContent(app)
{
}

void SaveLoad::Render(int selectedPid)
{
    // Sol tarafta kaydetme
    ImGui::BeginChild("Save", ImVec2(ImGui::GetWindowWidth() * 0.5f - 5.0f, 0), true);
    RenderSaveSection();
    ImGui::EndChild();

    ImGui::SameLine();

    // Sağ tarafta yükleme
    ImGui::BeginChild("Load", ImVec2(0, 0), true);
    RenderLoadSection();
    ImGui::EndChild();
}

void SaveLoad::RenderSaveSection()
{
    ImGui::Text("Save Configuration");
    ImGui::Separator();

    static char configName[128] = "";
    ImGui::InputText("Config Name", configName, sizeof(configName));

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Kaydedilecek ayarlar
    ImGui::Text("Settings to Save");
    
    static bool saveHotkeys = true;
    ImGui::Checkbox("Hotkeys", &saveHotkeys);
    
    static bool saveWindowLayout = true;
    ImGui::Checkbox("Window Layout", &saveWindowLayout);
    
    static bool saveBotSettings = true;
    ImGui::Checkbox("Bot Settings", &saveBotSettings);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Save Configuration", ImVec2(150, 30)))
    {
        // TODO: Kaydetme işlemleri
    }
}

void SaveLoad::RenderLoadSection()
{
    ImGui::Text("Load Configuration");
    ImGui::Separator();

    // Kayıtlı konfigürasyonlar listesi
    ImGui::BeginChild("ConfigList", ImVec2(0, ImGui::GetWindowHeight() * 0.7f), true);
    
    static int selectedConfig = -1;
    const char* configs[] = { "Config 1", "Config 2", "Config 3" }; // TODO: Gerçek konfigürasyonları listele
    
    for (int i = 0; i < 3; i++)
    {
        if (ImGui::Selectable(configs[i], selectedConfig == i))
        {
            selectedConfig = i;
        }
    }
    
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Yükleme ve silme butonları
    ImGui::BeginGroup();
    if (ImGui::Button("Load Selected", ImVec2(150, 30)))
    {
        // TODO: Seçili konfigürasyonu yükle
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Delete Selected", ImVec2(150, 30)))
    {
        // TODO: Seçili konfigürasyonu sil
    }
    ImGui::EndGroup();
}

}
} 