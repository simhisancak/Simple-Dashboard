#include "UIManager.h"
#include "../Application.h"
#include "../Renderer.h"
#include "../Auth/AuthManager.h"
#include "Views/Login/LoginView.h"
#include "Views/Dashboard/View.h"
#include "View.h"

UIManager::UIManager(Application* app, Renderer* renderer)
    : m_App(app), m_Renderer(renderer)
{
    RegisterView<UI::Views::LoginView>();
    RegisterView<UI::Views::DashboardView>();
}

UIManager::~UIManager() = default;

void UIManager::Update()
{
    if (m_App->GetAuthManager()->IsLoggedIn())
    {
        if (auto* loginView = GetView<UI::Views::LoginView>())
        {
            loginView->Update();
        }
    }
    else
    {
        if (auto* dashboardView = GetView<UI::Views::DashboardView>())
        {
            dashboardView->Update();
        }
    }
}

void UIManager::Render()
{
    if (m_App->GetAuthManager()->IsLoggedIn())
    {
        if (auto* loginView = GetView<UI::Views::LoginView>())
        {
            loginView->Render();
        }
    }
    else
    {
        if (auto* dashboardView = GetView<UI::Views::DashboardView>())
        {
            dashboardView->Render();
        }
    }
} 