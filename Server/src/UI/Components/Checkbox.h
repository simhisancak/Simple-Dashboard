#pragma once
#include "Component.h"
#include <functional>

namespace FracqServer {
namespace UI {
    namespace Components {

        class Checkbox : public Component {
        public:
            Checkbox(const char* label, bool* value)
                : m_Label(label)
                , m_Value(value) { }

            ~Checkbox() override = default;

            void Render() override {
                UpdatePosition();

                if (ImGui::Checkbox(m_Label, m_Value)) {
                    if (m_OnChangeCallback) {
                        m_OnChangeCallback(*m_Value);
                    }
                }
            }

            void SetOnChange(std::function<void(bool)> callback) {
                m_OnChangeCallback = std::move(callback);
            }

        private:
            const char* m_Label;
            bool* m_Value;
            std::function<void(bool)> m_OnChangeCallback;
        };

    } // namespace FracqServer::UI::Components
} // namespace FracqServer::UI
} // namespace FracqServer
