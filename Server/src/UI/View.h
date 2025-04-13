#pragma once

#include <string>
#include <memory>
#include <imgui.h>
#include "../Application.h"
#include "../Renderer.h"

namespace UI
{
    namespace Views
    {

        class IViewState;

        class View
        {
        public:
            View(Application *app, Renderer *renderer);
            virtual ~View() = default;

            virtual void Update() = 0;
            virtual void Render() = 0;

            virtual const char *GetViewId() const = 0;
            virtual std::unique_ptr<IViewState> CreateState() = 0;

            const std::string &GetName() const { return m_Name; }

            static const char *GetStaticViewId() { return "View"; }

        protected:
            Application *m_App;
            Renderer *m_Renderer;
            std::string m_Name;

            void RenderTitleBar(const char *title, bool showExitButton = true);

        protected:
            bool m_IsDragging = false;
            float m_LastMouseX = 0.0f;
            float m_LastMouseY = 0.0f;
        };

        class IViewState
        {
        public:
            virtual ~IViewState() = default;
            virtual void Reset() = 0;
        };

    }
}
