#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "View.h"

class Application;
class Renderer;

class UIManager {
   public:
    UIManager(Application* app, Renderer* renderer);
    ~UIManager();

    void Update();
    void Render();

    template <typename T>
    T* GetView() {
        static_assert(std::is_base_of<UI::Views::View, T>::value,
                      "T must derive from UI::Views::View");
        auto viewId = T::GetStaticViewId();
        auto it = m_Views.find(viewId);
        return it != m_Views.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }

    template <typename T>
    void RegisterView() {
        static_assert(std::is_base_of<UI::Views::View, T>::value,
                      "T must derive from UI::Views::View");
        auto view = std::make_unique<T>(m_App, m_Renderer);
        m_Views[view->GetViewId()] = std::move(view);
    }

   private:
    Application* m_App;
    Renderer* m_Renderer;

    std::unordered_map<const char*, std::unique_ptr<UI::Views::View>> m_Views;
};
