#pragma once

#include <string>
#include <Windows.h>

class Window
{
public:
    Window(const std::string &title, int width, int height);
    ~Window();

    void PollEvents();
    bool ShouldClose() const;
    HWND GetHandle() const { return m_Hwnd; }
    void ResizeToContent();

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_Hwnd = nullptr;
    std::string m_Title;
    int m_Width = 0;
    int m_Height = 0;
    bool m_ShouldClose = false;
};