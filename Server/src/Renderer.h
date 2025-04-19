#pragma once

#include "Window.h"
#include <d3d11.h>
#include <dxgi.h>
#include <memory>

class Renderer {
public:
    Renderer(Window* window);
    ~Renderer();

    void Begin();
    void End();
    Window* GetWindow() const { return m_Window; }

    ID3D11Device* GetDevice() const { return m_Device; }
    ID3D11DeviceContext* GetDeviceContext() const { return m_DeviceContext; }

private:
    void CreateDeviceAndSwapChain();
    void CreateRenderTarget();

private:
    Window* m_Window = nullptr;
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;
    IDXGISwapChain* m_SwapChain = nullptr;
    ID3D11RenderTargetView* m_RenderTargetView = nullptr;
};