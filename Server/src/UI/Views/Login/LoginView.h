#pragma once

#include "../../View.h"
#include "../../Components/Button.h"
#include "../../Components/Checkbox.h"
#include "../../Components/InputText.h"
#include "../../Components/Layout.h"
#include <string>
#include <memory>

namespace UI {
    namespace Views {
        class LoginViewState : public IViewState {
        public:
            void Reset() override {
                username.clear();
                password.clear();
                showPassword = false;
            }

            std::string username;
            std::string password;
            bool showPassword;
        };

        class LoginView : public View {
        public:
            LoginView(Application* app, Renderer* renderer);
            ~LoginView() override = default;

            void Update() override;
            void Render() override;

            const char* GetViewId() const override;
            std::unique_ptr<IViewState> CreateState() override;

            static const char* GetStaticViewId() { return "LoginView"; }

            LoginViewState* GetState();

        private:
            void RenderLoginForm();
            std::unique_ptr<LoginViewState> m_State;
            std::unique_ptr<Components::Layout> m_FormLayout;
            std::shared_ptr<Components::InputText> m_UsernameInput;
            std::shared_ptr<Components::InputText> m_PasswordInput;
            std::shared_ptr<Components::Checkbox> m_ShowPasswordCheckbox;
            std::shared_ptr<Components::Button> m_LoginButton;
        };
    }
} 