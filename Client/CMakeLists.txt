cmake_minimum_required(VERSION 3.10)
project(FracqClient VERSION 1.0.0)

# Force x86 architecture
set(CMAKE_GENERATOR_PLATFORM Win32)
set(CMAKE_VS_PLATFORM_NAME Win32)
set(CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE x86)

# Release build settings
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CONFIGURATION_TYPES Release)

# Compiler flags and definitions
if(MSVC)
    # Basic compiler flags with security disabled for DLL
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:IA32 /GS- /MT")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG /Oi /Gy")
    
    # Common definitions
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    add_definitions(-DRELEASE)
    
    # Disable export of symbols for DLL
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows için gerekli tanımlamaları ekle
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    add_definitions(-DRELEASE)
endif()

# Shared directory path
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../shared)

# Feature kaynak dosyaları
set(FEATURE_FILES
    src/features/Features.cpp
    src/features/farmbot/FarmBot.cpp
    src/features/main/Main.cpp
)

# Network kaynak dosyaları
set(NETWORK_FILES
    src/network/Network.cpp
)

# Common kaynak dosyaları
set(COMMON_FILES
    src/common/Logger.cpp
    src/common/Helper.cpp
)

# Hack kaynak dosyaları
set(HACK_FILES
    src/hack/globals/Globals.cpp
    src/hack/helper/Helper.cpp
    src/hack/instance/Instance.cpp
    src/hack/instance/InstanceHelper.cpp
    src/hack/item/Item.cpp
    src/hack/item/ItemHelper.cpp
    src/hack/groundItem/GroundItem.cpp
    src/hack/groundItem/GroundItemHelper.cpp
)

# Core kaynak dosyaları
set(CORE_FILES
    src/core/main.cpp
    src/core/ClientApp.cpp
)

# Tüm kaynak dosyalarını birleştir
set(SOURCE_FILES
    ${CORE_FILES}
    ${NETWORK_FILES}
    ${COMMON_FILES}
    ${HACK_FILES}
    ${FEATURE_FILES}
)

# Build as an injectable DLL (no exports)
add_library(FracqClient SHARED ${SOURCE_FILES})

# Disable DLL exports and add security settings
if(MSVC)
    set_target_properties(FracqClient PROPERTIES
        ENABLE_EXPORTS OFF
        WINDOWS_EXPORT_ALL_SYMBOLS OFF
        LINK_FLAGS "/INCREMENTAL:NO /OPT:REF /OPT:ICF /DYNAMICBASE:NO /NXCOMPAT /SAFESEH:NO /LARGEADDRESSAWARE:NO"
    )
endif()

# Include dizinlerini ekleyin
target_include_directories(FracqClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SHARED_DIR}
)

# Windows için output ayarları
if(WIN32)
    set_target_properties(FracqClient PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        PREFIX ""
        SUFFIX ".dll"
        COMPILE_PDB_NAME "FracqClient"
        COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    # Force static runtime libraries for Release
    set_property(TARGET FracqClient PROPERTY 
        MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

# Windows kütüphanelerini ekle
target_link_libraries(FracqClient PRIVATE
    kernel32
    user32
    ws2_32
    libcmt
    libvcruntime
    libucrt
) 