#pragma once

#include "../Application.h"
#include "../Renderer.h"
#include <imgui.h>
#include <memory>
#include <string>

namespace UI {
namespace Views {

    class IViewState;

    class View {
    public:
        View(Application* app, Renderer* renderer);
        virtual ~View() = default;

        virtual void Update() = 0;
        virtual void Render() = 0;

        virtual const char* GetViewId() const = 0;
        virtual std::unique_ptr<IViewState> CreateState() = 0;

        const std::string& GetName() const { return m_Name; }

        static const char* GetStaticViewId() { return "View"; }

    protected:
        Application* m_App;
        Renderer* m_Renderer;
        std::string m_Name;

        void RenderTitleBar(const char* title, bool showExitButton = true);

    private:
        void RenderInjectModal();
        bool TryInject(const char* processName, DWORD pid);

        bool m_IsDragging = false;
        float m_LastMouseX = 0.0f;
        float m_LastMouseY = 0.0f;
        bool m_ShowInjectModal = false;

        std::string m_LastError;
        bool m_UseProcessName = true;
    };

    class IViewState {
    public:
        virtual ~IViewState() = default;
        virtual void Reset() = 0;
    };

}
}
