#include <windows.h>
#include <memory>
#include <iostream>
#include <string>
#include <conio.h>
#include "ClientApp.h"
#include "common/Logger.h"

#define HEALTH_CHECK_INTERVAL 5000
#define HEALTH_CHECK_RETRY_INTERVAL 1000
#define KEY_CHECK_INTERVAL 100
#define INITIAL_SLEEP_TIME 500
#define MAX_RECONNECT_ATTEMPTS 3
#define MAX_FAILED_HEALTH_CHECKS 2

std::unique_ptr<FracqClient::ClientApp> g_ClientApp;
HANDLE g_Thread = NULL;
HMODULE g_Module = NULL;

void UnloadDLL() {
    LOG_INFO(LOG_COMPONENT_CONSOLE, "Unloading DLL...");

    if (g_ClientApp) {
        g_ClientApp.reset();
    }

    Common::Logger::Cleanup();

    if (g_Module) {
        FreeLibraryAndExitThread(g_Module, 0);
    }
}

DWORD WINAPI ClientThread(LPVOID lpParam) {
    g_Module = static_cast<HMODULE>(lpParam);
    Sleep(INITIAL_SLEEP_TIME);

    try {
        char processName[MAX_PATH] = { 0 };
        GetModuleFileNameA(NULL, processName, MAX_PATH);
        LOG_INFO(LOG_COMPONENT_CONSOLE, "Host process: " << processName);

        LOG_INFO(LOG_COMPONENT_CONSOLE, "Creating client application...");
        std::cout.flush();

        g_ClientApp = std::make_unique<FracqClient::ClientApp>();

        LOG_INFO(LOG_COMPONENT_CONSOLE, "Initializing network...");
        if (!g_ClientApp->Initialize()) {
            LOG_ERROR(LOG_COMPONENT_CONSOLE,
                      "Failed to initialize client: " << g_ClientApp->GetLastError());
            UnloadDLL();
            return 1;
        }
        LOG_INFO(LOG_COMPONENT_CONSOLE, "Network initialized successfully.");

        bool exitRequested = false;
        int failedHealthChecks = 0;
        int failedReconnectAttempts = 0;
        DWORD lastHealthCheckTime = GetTickCount();
        DWORD lastKeyCheckTime = GetTickCount();

        while (!exitRequested) {
            DWORD currentTime = GetTickCount();
            if (currentTime - lastKeyCheckTime >= KEY_CHECK_INTERVAL) {
                lastKeyCheckTime = currentTime;
                if (GetAsyncKeyState(VK_DELETE) & 0x8000) {
                    LOG_INFO(LOG_COMPONENT_CONSOLE, "Delete key pressed - unloading DLL...");
                    UnloadDLL();
                    return 0;
                }
            }

            if (!g_ClientApp->IsRegistered()) {
                LOG_INFO(LOG_COMPONENT_CONSOLE, "\n=== CONNECTION ATTEMPT ===\n");
                LOG_INFO(LOG_COMPONENT_CONSOLE,
                         "Connecting to server and sending registration packet...");

                if (!g_ClientApp->RegisterWithServer()) {
                    LOG_ERROR(LOG_COMPONENT_CONSOLE,
                              "Failed to register with server: " << g_ClientApp->GetLastError());
                    failedReconnectAttempts++;

                    if (failedReconnectAttempts >= MAX_RECONNECT_ATTEMPTS) {
                        LOG_ERROR(LOG_COMPONENT_CONSOLE,
                                  "Failed to reconnect after " << MAX_RECONNECT_ATTEMPTS
                                                               << " attempts.");
                        UnloadDLL();
                        return 1;
                    }

                    LOG_INFO(LOG_COMPONENT_CONSOLE,
                             "Will try again in 5 seconds (attempt "
                                 << failedReconnectAttempts << "/" << MAX_RECONNECT_ATTEMPTS
                                 << ")...");
                    Sleep(HEALTH_CHECK_INTERVAL);
                    continue;
                }

                LOG_INFO(LOG_COMPONENT_CONSOLE, "Registration successful!");
                LOG_INFO(LOG_COMPONENT_CONSOLE,
                         "Connected to server. Heartbeats will be sent every "
                             << HEALTH_CHECK_INTERVAL / 1000 << " seconds.");
                LOG_INFO(LOG_COMPONENT_CONSOLE, "Press Delete to unload DLL.");
                failedReconnectAttempts = 0;
                failedHealthChecks = 0;
                lastHealthCheckTime = GetTickCount();
            }

            if (currentTime - lastHealthCheckTime >= HEALTH_CHECK_INTERVAL) {
                lastHealthCheckTime = currentTime;

                if (!g_ClientApp->SendHealthCheck()) {
                    failedHealthChecks++;
                    LOG_WARN(LOG_COMPONENT_CONSOLE,
                             "Health check failed (" << failedHealthChecks << "/"
                                                     << MAX_FAILED_HEALTH_CHECKS << ")");

                    if (failedHealthChecks >= MAX_FAILED_HEALTH_CHECKS) {
                        LOG_WARN(LOG_COMPONENT_CONSOLE,
                                 "Multiple health checks failed. Attempting to reconnect...");
                        g_ClientApp->ResetRegistration();
                        failedHealthChecks = 0;
                        Sleep(HEALTH_CHECK_RETRY_INTERVAL);
                        continue;
                    }
                } else {
                    failedHealthChecks = 0;
                }
            }

            Sleep(10);
        }
    } catch (const std::exception& e) {
        LOG_ERROR(LOG_COMPONENT_CONSOLE, "Exception occurred: " << e.what());
        UnloadDLL();
        return 1;
    }

    return 0;
}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HMODULE hModule,
                                                     DWORD dwReason,
                                                     LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        g_Thread = CreateThread(NULL, 0, ClientThread, hModule, 0, NULL);
        if (g_Thread == NULL) {
            OutputDebugStringA("Fracq: Failed to create client thread");
            return FALSE;
        }
        OutputDebugStringA("Fracq: Client thread created successfully");
        break;

    case DLL_PROCESS_DETACH:
        if (g_Thread != NULL) {
            CloseHandle(g_Thread);
            g_Thread = NULL;
        }
        break;
    }
    return TRUE;
}