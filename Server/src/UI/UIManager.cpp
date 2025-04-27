#include "UIManager.h"
#include "../Application.h"
#include "../Renderer.h"
#include "../Auth/AuthManager.h"
#include "Views/Login/LoginView.h"
#include "Views/Dashboard/View.h"
#include "View.h"

namespace FracqServer {
namespace UI {

    UIManager::UIManager(Application* app, Renderer* renderer)
        : m_App(app)
        , m_Renderer(renderer) {
        RegisterView<UI::Views::LoginView>();
        RegisterView<UI::Views::DashboardView>();
    }

    UIManager::~UIManager() = default;

    void UIManager::Update() {
        if (!m_App->GetAuthManager()->IsLoggedIn()) { // Login olmamışsa
            if (auto* loginView = GetView<UI::Views::LoginView>()) {
                loginView->Update();
            }
        } else { // Login olmuşsa
            if (auto* dashboardView = GetView<UI::Views::DashboardView>()) {
                dashboardView->Update();
            }
        }
    }

    void UIManager::Render() {
        if (!m_App->GetAuthManager()->IsLoggedIn()) { // Login olmamışsa
            if (auto* loginView = GetView<UI::Views::LoginView>()) {
                loginView->Render();
            }
        } else { // Login olmuşsa
            if (auto* dashboardView = GetView<UI::Views::DashboardView>()) {
                dashboardView->Render();
            }
        }
    }

} // namespace UI
} // namespace FracqServer