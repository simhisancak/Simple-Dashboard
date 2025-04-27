#include "Renderer.h"
#include "Window.h"
#include <stdexcept>
#include <string>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

namespace FracqServer {

std::string GetDXErrorMessage(HRESULT hr) {
    char buffer[256];
    sprintf_s(buffer, "DirectX Hata Kodu: 0x%08X", static_cast<unsigned int>(hr));
    return std::string(buffer);
}

Renderer::Renderer(Window* window)
    : m_Window(window) {
    CreateDeviceAndSwapChain();
    CreateRenderTarget();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.MouseDrawCursor = false;
    io.ConfigInputTextCursorBlink = false;
    io.ConfigMemoryCompactTimer = 60.0f;

    io.Fonts->Clear();
    ImFontConfig config;
    config.OversampleH = 2;
    config.OversampleV = 2;
    config.PixelSnapH = true;
    config.RasterizerMultiply = 1.5f;

    static const ImWchar ranges[] = { 0x0020, 0x00FF, // Basic Latin + Latin Supplement
                                      0x0100, 0x017F, // Latin Extended-A
                                      0x0180, 0x024F, // Latin Extended-B
                                      0x0300, 0x036F, // Combining Diacritical Marks
                                      0x00C7, 0x00C7, // Latin Capital Letter C with Cedilla
                                      0x00E7, 0x00E7, // Latin Small Letter C with Cedilla
                                      0x011E, 0x011E, // Latin Capital Letter G with Breve
                                      0x011F, 0x011F, // Latin Small Letter G with Breve
                                      0x0130, 0x0130, // Latin Capital Letter I with Dot Above
                                      0x0131, 0x0131, // Latin Small Letter Dotless I
                                      0x015E, 0x015E, // Latin Capital Letter S with Cedilla
                                      0x015F, 0x015F, // Latin Small Letter S with Cedilla
                                      0x0000 };

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 15.0f, &config, ranges);

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 2.0f;
    style.ChildRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 2.0f;
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    style.CurveTessellationTol = 1.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.94f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.32f, 0.32f, 0.32f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.28f, 0.28f, 0.67f);

    ImGui_ImplWin32_Init(m_Window->GetHandle());
    ImGui_ImplDX11_Init(m_Device, m_DeviceContext);
}

Renderer::~Renderer() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (m_RenderTargetView) {
        m_RenderTargetView->Release();
        m_RenderTargetView = nullptr;
    }
    if (m_SwapChain) {
        m_SwapChain->Release();
        m_SwapChain = nullptr;
    }
    if (m_DeviceContext) {
        m_DeviceContext->Release();
        m_DeviceContext = nullptr;
    }
    if (m_Device) {
        m_Device->Release();
        m_Device = nullptr;
    }
}

void Renderer::Begin() {
    static const float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, clearColor);
    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Renderer::End() {
    ImGui::Render();

    m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    HRESULT hr = m_SwapChain->Present(1, 0);

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to present swap chain: " + GetDXErrorMessage(hr));
    }
}

void Renderer::CreateDeviceAndSwapChain() {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = m_Window->GetWidth();
    sd.BufferDesc.Height = m_Window->GetHeight();
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_Window->GetHandle();
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
    };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                               D3D_DRIVER_TYPE_HARDWARE,
                                               nullptr,
                                               createDeviceFlags,
                                               featureLevelArray,
                                               2,
                                               D3D11_SDK_VERSION,
                                               &sd,
                                               &m_SwapChain,
                                               &m_Device,
                                               &featureLevel,
                                               &m_DeviceContext);

    if (FAILED(hr)) {
        hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                           D3D_DRIVER_TYPE_WARP,
                                           nullptr,
                                           createDeviceFlags,
                                           featureLevelArray,
                                           2,
                                           D3D11_SDK_VERSION,
                                           &sd,
                                           &m_SwapChain,
                                           &m_Device,
                                           &featureLevel,
                                           &m_DeviceContext);

        if (FAILED(hr))
            throw std::runtime_error("Failed to create device and swap chain: "
                                     + GetDXErrorMessage(hr));
    }

    IDXGIDevice1* dxgiDevice;
    if (SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)))) {
        dxgiDevice->SetMaximumFrameLatency(1);
        dxgiDevice->Release();
    }

    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.ScissorEnable = true;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.AntialiasedLineEnable = false;

    ID3D11RasterizerState* rasterizerState;
    hr = m_Device->CreateRasterizerState(&rasterDesc, &rasterizerState);
    if (SUCCEEDED(hr)) {
        m_DeviceContext->RSSetState(rasterizerState);
        rasterizerState->Release();
    }
}

void Renderer::CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    HRESULT hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr))
        throw std::runtime_error("Failed to get back buffer: " + GetDXErrorMessage(hr));

    hr = m_Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_RenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        throw std::runtime_error("Failed to create render target view: " + GetDXErrorMessage(hr));
}

} // namespace FracqServer