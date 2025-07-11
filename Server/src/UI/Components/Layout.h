#pragma once
#include "Component.h"
#include <vector>
#include <memory>

namespace FracqServer {
namespace UI {
    namespace Components {

        enum class FlexDirection { Row, Column };

        class Layout : public Component {
        public:
            Layout(FlexDirection direction = FlexDirection::Column)
                : m_Direction(direction)
                , m_Spacing(ImGui::GetStyle().ItemSpacing.y)
                , m_Padding(ImGui::GetStyle().FramePadding.y) { }

            void Render() override {
                if (m_Components.empty())
                    return;

                UpdatePosition();

                float nextX = m_Position.x + m_Padding;
                float nextY = m_Position.y + m_Padding;

                for (const auto& component : m_Components) {
                    component->SetPosition(nextX, nextY);

                    component->Render();

                    if (m_Direction == FlexDirection::Column) {
                        nextY += component->GetSize().y + m_Spacing;
                    } else {
                        nextX += component->GetSize().x + m_Spacing;
                    }
                }

                if (m_Direction == FlexDirection::Column) {
                    m_Size.x = m_Components[0]->GetSize().x;
                    m_Size.y = nextY - m_Position.y;
                } else {
                    m_Size.x = nextX - m_Position.x;
                    m_Size.y = m_Components[0]->GetSize().y;
                }
            }

            void AddComponent(std::shared_ptr<Component> component) {
                if (component) {
                    m_Components.push_back(component);
                }
            }

            void SetSpacing(float spacing) { m_Spacing = spacing; }

            void SetPadding(float padding) { m_Padding = padding; }

            void Clear() { m_Components.clear(); }

        private:
            FlexDirection m_Direction;
            std::vector<std::shared_ptr<Component>> m_Components;
            float m_Spacing;
            float m_Padding;
        };

    } // namespace Components
} // namespace UI
} // namespace FracqServer
