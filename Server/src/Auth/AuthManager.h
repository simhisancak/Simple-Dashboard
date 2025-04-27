#pragma once

#include <string>

class AuthManager {
public:
    AuthManager();
    ~AuthManager() = default;

    bool Login(const std::string& username, const std::string& password);
    void Logout();
    bool IsLoggedIn() const { return m_IsLoggedIn; }
    const std::string& GetLastError() const { return m_LastError; }

private:
    bool m_IsLoggedIn = true;
    std::string m_LastError;
};