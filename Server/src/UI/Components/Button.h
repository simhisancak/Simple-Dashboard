#pragma once
#include "Component.h"
#include <functional>

namespace UI::Components {

class Button : public Component {
public:
    Button(const char* label) 
        : m_Label(label)
    {
    }

    ~Button() override = default;

    void Render() override {
        UpdatePosition();
        
        if (ImGui::Button(m_Label, m_Size))
        {
            if (m_ClickCallback) {
                m_ClickCallback();
            }
        }
    }

    void SetOnClick(std::function<void()> callback) {
        m_ClickCallback = std::move(callback);
    }

private:
    const char* m_Label;
    std::function<void()> m_ClickCallback;
};

} // namespace UI::Components 