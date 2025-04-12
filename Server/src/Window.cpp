#include "Window.h"
#include <stdexcept>
#include <dwmapi.h>

#include "imgui.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::Window(const std::string& title, int width, int height)
    : m_Title(title), m_Width(width), m_Height(height)
{
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"FracqWindowClass";
    wc.hbrBackground = NULL;
    RegisterClassExW(&wc);

    DWORD style = WS_POPUP | WS_VISIBLE;
    DWORD exStyle = WS_EX_LAYERED;
    
    RECT rect = { 0, 0, m_Width, m_Height };
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - (rect.right - rect.left)) / 2;
    int posY = (screenHeight - (rect.bottom - rect.top)) / 2;

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, title.c_str(), (int)title.size(), NULL, 0);
    std::wstring wTitle(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, title.c_str(), (int)title.size(), &wTitle[0], size_needed);

    m_Hwnd = CreateWindowExW(
        exStyle,
        L"FracqWindowClass",
        wTitle.c_str(),
        style,
        posX, posY,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, this
    );

    if (!m_Hwnd)
        throw std::runtime_error("Window creation failed!");

    SetLayeredWindowAttributes(m_Hwnd, 0, 255, LWA_ALPHA);
    ShowWindow(m_Hwnd, SW_SHOW);
    UpdateWindow(m_Hwnd);
}

void Window::ResizeToContent()
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize = ImGui::GetWindowSize();
    
    RECT rect = { 0, 0, static_cast<LONG>(windowSize.x), static_cast<LONG>(windowSize.y) };
    AdjustWindowRectEx(&rect, GetWindowLong(m_Hwnd, GWL_STYLE), FALSE, GetWindowLong(m_Hwnd, GWL_EXSTYLE));
    
    SetWindowPos(m_Hwnd, NULL, 0, 0, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
        SWP_NOMOVE | SWP_NOZORDER);
    
    m_Width = static_cast<int>(windowSize.x);
    m_Height = static_cast<int>(windowSize.y);
}

Window::~Window()
{
    if (m_Hwnd)
    {
        DestroyWindow(m_Hwnd);
        m_Hwnd = nullptr;
    }
    
    UnregisterClassW(L"FracqWindowClass", GetModuleHandle(NULL));
}

void Window::PollEvents()
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        if (msg.message == WM_QUIT)
        {
            m_ShouldClose = true;
        }
    }
}

bool Window::ShouldClose() const
{
    return m_ShouldClose;
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
        return true;

    Window* window = nullptr;
    
    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = static_cast<Window*>(create->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    }
    else
    {
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    switch (uMsg)
    {
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_SCREENSAVE || (wParam & 0xFFF0) == SC_MONITORPOWER)
            return 0;
        break;

    case WM_LBUTTONDOWN:
        break;

    case WM_MOUSEMOVE:
        break;

    case WM_LBUTTONUP:
        break;
        
    case WM_CLOSE:
        if (window) window->m_ShouldClose = true;
        return 0;
        
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        
    case WM_KEYDOWN:
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}