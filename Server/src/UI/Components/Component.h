#pragma once
#include <string>
#include "imgui.h"

namespace UI::Components {

class Component {
public:
    Component() 
        : m_Position(0, 0)
        , m_Size(0, 0)
        , m_AutoPosition(true)
    {}

    virtual ~Component() = default;
    
    // Her component'in kendi render fonksiyonu olmalı
    virtual void Render() = 0;
    
    // Component pozisyonunu ayarla
    void SetPosition(float x, float y) {
        m_Position = ImVec2(x, y);
    }
    
    // Component boyutunu ayarla
    void SetSize(float width, float height) {
        m_Size = ImVec2(width, height);
    }

    const ImVec2& GetPosition() const { return m_Position; }
    const ImVec2& GetSize() const { return m_Size; }

    void EnableAutoPosition(bool enable = true, float fixedX = -1) { 
        m_AutoPosition = enable; 
    }
    bool IsAutoPositioned() const { return m_AutoPosition; }

protected:
    ImVec2 m_Position;
    ImVec2 m_Size;
    bool m_AutoPosition;

    void UpdatePosition() {
        ImGui::SetCursorPos(m_Position);
    }
};

} // namespace UI::Components 