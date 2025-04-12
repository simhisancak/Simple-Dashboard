#include "LoginView.h"
#include "Application.h"
#include "Renderer.h"
#include "Auth/AuthManager.h"
#include "UI/Components/Button.h"
#include "UI/Components/Checkbox.h"
#include "UI/Components/InputText.h"
#include "imgui.h"
#include <algorithm>

namespace UI::Views {

LoginView::LoginView(Application* app, Renderer* renderer)
    : View(app, renderer)
    , m_State(std::make_unique<LoginViewState>())
{
    m_State->Reset();
    
    m_UsernameInput = std::make_shared<Components::InputText>("Kullanıcı Adı", &m_State->username);
    m_PasswordInput = std::make_shared<Components::InputText>("Şifre", &m_State->password, 
        m_State->showPassword ? ImGuiInputTextFlags_EnterReturnsTrue : ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);
    m_ShowPasswordCheckbox = std::make_shared<Components::Checkbox>("Şifreyi Göster", &m_State->showPassword);
    m_LoginButton = std::make_shared<Components::Button>("Giriş Yap");
    
    m_FormLayout = std::make_unique<Components::Layout>(Components::FlexDirection::Column);
    m_FormLayout->AddComponent(m_UsernameInput);
    m_FormLayout->AddComponent(m_PasswordInput);
    m_FormLayout->AddComponent(m_ShowPasswordCheckbox);
    m_FormLayout->AddComponent(m_LoginButton);
    
    m_PasswordInput->SetOnEnterPressed([this]() {
        if (m_App->GetAuthManager()->Login(m_State->username.c_str(), m_State->password.c_str()))
        {
            m_State->Reset();
        }
    });
    
    m_LoginButton->SetOnClick([this]() {
        if (m_App->GetAuthManager()->Login(m_State->username.c_str(), m_State->password.c_str()))
        {
            m_State->Reset();
        }
    });
}

void LoginView::Update()
{
    if (m_App->GetAuthManager()->IsLoggedIn())
    {
        m_State->Reset();
    }
}

void LoginView::Render()
{
    float windowWidth = static_cast<float>(m_App->GetWidth());
    float windowHeight = static_cast<float>(m_App->GetHeight());
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

    ImGui::Begin("Giriş", nullptr, 
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

    RenderTitleBar("Giriş", true);

    RenderLoginForm();
    
    m_Renderer->GetWindow()->ResizeToContent();
    
    ImGui::End();
}

LoginViewState* LoginView::GetState()
{
    return m_State.get();
}

const char* LoginView::GetViewId() const {
    return GetStaticViewId();
}

std::unique_ptr<UI::Views::IViewState> LoginView::CreateState() {
    return std::make_unique<LoginViewState>();
}

} // namespace UI::Views 