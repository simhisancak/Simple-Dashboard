#include "Application.h"
#include "Auth/AuthManager.h"
#include "Renderer.h"
#include "Server/ServerManager.h"
#include "UI/Theme.h"
#include "UI/UIManager.h"
#include "Utils/Timer/Timer.h"
#include "Window.h"
#include "imgui.h"

#include <chrono>
#include <iostream>
#include <thread>

namespace FracqServer {

Application::Application(const std::string& name, int width, int height)
    : m_Name(name)
    , m_Width(width)
    , m_Height(height) {
    std::cout << "Starting Fracq Application..." << std::endl;

    try {
        m_Window = std::make_unique<Window>(name, width, height);
        m_Renderer = std::make_unique<Renderer>(m_Window.get());
        m_AuthManager = std::make_unique<::AuthManager>();
        m_UIManager = std::make_unique<UI::UIManager>(this, m_Renderer.get());
        m_ServerManager = std::make_unique<Server::ServerManager>(this);

        UI::Theme::Apply();

        if (!m_ServerManager->Start(8888)) {
            std::cerr << "Failed to start Server Manager!" << std::endl;
        }

        std::cout << "Fracq Application started successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during application startup: " << e.what() << std::endl;
        throw;
    }
}

Application::~Application() { Shutdown(); }

void Application::Run() {
    m_Running = true;
    m_LastFrameTime = Utils::Timer::GetTime();

    std::cout << "Application loop started." << std::endl;

    while (m_Running) {
        if (m_Window->ShouldClose()) {
            m_Running = false;
            break;
        }

        try {
            Update();
            Render();
            LimitFrameRate();
        } catch (const std::exception& e) {
            std::cerr << "Error in application loop: " << e.what() << std::endl;
            m_Running = false;
            break;
        }
    }

    std::cout << "Application loop terminated." << std::endl;
}

void Application::Shutdown() {
    if (!m_Running)
        return;

    m_Running = false;

    std::cout << "Cleaning up application resources..." << std::endl;

    m_UIManager.reset();

    if (m_ServerManager) {
        m_ServerManager->Stop();
        m_ServerManager.reset();
    }

    m_Renderer.reset();
    m_Window.reset();

    std::cout << "Application resources cleaned up." << std::endl;
}

void Application::Update() {
    m_Window->PollEvents();
    if (m_UIManager)
        m_UIManager->Update();
}

void Application::Render() {
    m_Renderer->Begin();
    if (m_UIManager)
        m_UIManager->Render();
    m_Renderer->End();
}

void Application::LimitFrameRate() {
    double currentTime = Utils::Timer::GetTime();
    double deltaTime = currentTime - m_LastFrameTime;

    if (m_TargetFrameTime > 0.0) {
        if (deltaTime < m_TargetFrameTime) {
            double sleepTime = m_TargetFrameTime - deltaTime;
            if (sleepTime > 0.002) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime - 0.002));
            }
            while (Utils::Timer::GetTime() - m_LastFrameTime < m_TargetFrameTime) {
                _mm_pause();
            }
            currentTime = Utils::Timer::GetTime();
        }
    }

    m_LastFrameTime = currentTime;
}

} // namespace FracqServer