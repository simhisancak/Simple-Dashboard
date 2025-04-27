#pragma once

#include "Component.h"
#include "Layout.h"
#include <string>
#include <memory>
#include <functional>

namespace FracqServer {
namespace UI {
    namespace Components {

        class ModalForm : public Component {
        public:
            ModalForm(const std::string& title, float width = 400.0f, float height = 300.0f);
            ~ModalForm() override = default;

            void Render() override;
            void SetSize(float width, float height);

            void Show() { m_IsVisible = true; }
            void Hide() { m_IsVisible = false; }
            bool IsVisible() const { return m_IsVisible; }

            void SetTitle(const std::string& title) { m_Title = title; }
            void SetLayout(std::shared_ptr<Layout> layout) { m_Layout = layout; }

            using ContentRenderer = std::function<void()>;
            void SetContentRenderer(ContentRenderer renderer) { m_ContentRenderer = renderer; }

            void SetPadding(float padding) { m_Padding = padding; }
            void SetSpacing(float spacing) { m_Spacing = spacing; }

            bool BeginContent();
            void EndContent();

        private:
            std::string m_Title;
            bool m_IsVisible;
            float m_Width;
            float m_Height;
            float m_Padding;
            float m_Spacing;
            std::shared_ptr<Layout> m_Layout;
            bool m_IsContentRendering;
            ContentRenderer m_ContentRenderer;
        };
    } // namespace Components
} // namespace UI
} // namespace FracqServer
