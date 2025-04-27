#include "Application.h"
#include <Windows.h>
#include <iostream>
#include <exception>
#include <cstdlib>

void ReportError(const char* message) {
    MessageBoxA(NULL, message, "Fracq - Hata", MB_OK | MB_ICONERROR);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    try {
        FracqServer::Application app("Fracq", 800, 600);
        app.Run();
        return 0;
    } catch (const std::exception& e) {
        ReportError(e.what());
        return -1;
    }
}