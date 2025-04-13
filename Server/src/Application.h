#pragma once

#include <string>
#include <memory>

// Forward declarations
class Window;
class Renderer;
class UIManager;
class AuthManager;

namespace Server
{
    class ServerManager; // Forward declaration for ServerManager
}

class Application
{
public:
    Application(const std::string &name, int width, int height);
    ~Application();

    void Run();
    void Shutdown();

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    const std::string &GetName() const { return m_Name; }
    bool IsLoggedIn() const { return m_LoggedIn; }
    void SetLoggedIn(bool loggedIn) { m_LoggedIn = loggedIn; }
    bool IsRunning() const { return m_Running; }
    AuthManager *GetAuthManager() const { return m_AuthManager.get(); }
    Server::ServerManager *GetServerManager() const { return m_ServerManager.get(); }

private:
    void Update();
    void Render();
    void LimitFrameRate();

private:
    std::string m_Name;
    int m_Width, m_Height;
    bool m_Running = false;
    bool m_LoggedIn = false;

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<UIManager> m_UIManager;
    std::unique_ptr<AuthManager> m_AuthManager;
    std::unique_ptr<Server::ServerManager> m_ServerManager;
    double m_TargetFrameTime = 1.0 / 60.0; // 60 FPS
    double m_LastFrameTime = 0.0;
};