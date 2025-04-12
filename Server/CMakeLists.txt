cmake_minimum_required(VERSION 3.10)
project(FracqServer VERSION 1.0.0)

# Force x86 architecture
set(CMAKE_GENERATOR_PLATFORM Win32)
set(CMAKE_VS_PLATFORM_NAME Win32)
set(CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE x86)

# Release build settings
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CONFIGURATION_TYPES Release)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags and definitions
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG /Oi /Gy")
    
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    add_definitions(-DRELEASE)
endif()

# ImGui dizini
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

# Shared directory path
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../shared)

# Server kaynak dosyaları
set(SERVER_FILES
    src/Server/ServerManager.cpp
)

set(CONTENT_FILES
    src/UI/Views/Dashboard/Content/MainContent.cpp
    src/UI/Views/Dashboard/Content/FarmBot/FarmBot.cpp
    src/UI/Views/Dashboard/Content/SaveLoad/SaveLoad.cpp
)

# View kaynak dosyaları
set(VIEW_FILES
    src/UI/Views/Login/LoginView.cpp
    src/UI/Views/Login/LoginForm.cpp
    src/UI/Views/Dashboard/View.cpp
    src/UI/Views/Dashboard/Header.cpp
    src/UI/Views/Dashboard/Sidebar.cpp
    ${CONTENT_FILES}
)

# Component kaynak dosyaları
set(COMPONENT_FILES
    src/UI/Components/Component.h
    src/UI/Components/Button.h
    src/UI/Components/Checkbox.h
    src/UI/Components/InputText.h
    src/UI/Components/ModalForm.cpp
    src/UI/Components/Table.h
)

# UI kaynak dosyaları
set(UI_FILES
    src/UI/UIManager.cpp
)

# Sadece temel kaynak dosyalarını ekleyelim
set(SOURCE_FILES
    src/main.cpp
    src/Application.cpp
    src/Renderer.cpp
    src/Window.cpp
    src/Auth/AuthManager.cpp
    src/UI/UIManager.cpp
    src/UI/View.cpp
    src/Utils/Timer/Timer.cpp
    src/Utils/Injector/ManualMap.cpp
    src/Utils/Injector/Injector.cpp
    src/common/Logger.cpp
    ${SERVER_FILES}
    ${VIEW_FILES}
    ${COMPONENT_FILES}
)

# ImGui kaynak dosyalarını ekleyin
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
)

# WIN32 uygulaması için ana giriş noktası
add_executable(FracqServer WIN32 ${SOURCE_FILES} ${IMGUI_SOURCES})

# Include dizinlerini ekleyin
target_include_directories(FracqServer PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SHARED_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# DirectX bağımlılıklarını ekleyin
target_link_libraries(FracqServer PRIVATE
    d3d11.lib
    dxgi.lib
    user32.lib
    gdi32.lib
    shell32.lib
    ws2_32.lib
)

# Windows için x86 platform ayarları
if(WIN32)
    set_target_properties(FracqServer PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
    )
endif()