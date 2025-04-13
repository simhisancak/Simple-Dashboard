#include "AuthManager.h"

AuthManager::AuthManager() = default;

bool AuthManager::Login(const std::string &username, const std::string &password)
{
    if (username.empty() || password.empty())
    {
        m_LastError = "Kullanıcı adı ve şifre boş olamaz!";
        return false;
    }

    if (username == "admin" && password == "admin") // Örnek giriş
    {
        m_IsLoggedIn = true;
        m_LastError.clear();
        return true;
    }

    m_LastError = "Geçersiz kullanıcı adı veya şifre!";
    return false;
}

void AuthManager::Logout()
{
    m_IsLoggedIn = false;
    m_LastError.clear();
}