#include "Logger.h"

namespace Common {

std::mutex Logger::s_Mutex;
std::ofstream Logger::s_LogFile;
bool Logger::s_Initialized = false;
HANDLE Logger::s_ConsoleHandle = INVALID_HANDLE_VALUE;

void Logger::Initialize() {
    if (!s_Initialized) {
        if (LOG_CONSOLE) {
            FreeConsole();
            
            if (AllocConsole()) {
                FILE* pConIn = nullptr;
                FILE* pConOut = nullptr;
                FILE* pConErr = nullptr;
                
                freopen_s(&pConIn, "CONIN$", "r", stdin);
                freopen_s(&pConOut, "CONOUT$", "w", stdout);
                freopen_s(&pConErr, "CONOUT$", "w", stderr);
                
                std::cout.clear();
                std::cerr.clear();
                
                SetConsoleTitleA("Fracq Client Console");
                
                s_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
                
                ShowWindow(GetConsoleWindow(), SW_SHOW);
                
                SetConsoleTextAttribute(s_ConsoleHandle, COLOR_INFO);
                std::cout << "\n=== Fracq Client Console Initialized ===\n" << std::endl;
                SetConsoleTextAttribute(s_ConsoleHandle, COLOR_DEFAULT);
            }
        }

        if (LOG_TO_FILE) {
            auto now = std::time(nullptr);
            auto tm = *std::localtime(&now);
            char filename[64];
            strftime(filename, sizeof(filename), "fracq_%Y%m%d_%H%M%S.log", &tm);
            s_LogFile.open(filename, std::ios::out | std::ios::app);
        }
        s_Initialized = true;
    }
}

void Logger::SetConsoleColor(int color) {
    if (s_ConsoleHandle != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(s_ConsoleHandle, color);
    }
}

void Logger::Log(int level, int component, const std::string& msg) {
    std::lock_guard<std::mutex> lock(s_Mutex);
    Initialize();

    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", &tm);

    const char* levelName;
    int color;
    switch (level) {
        case LOG_LEVEL_DEBUG:
            levelName = "DEBUG";
            color = COLOR_DEBUG;
            break;
        case LOG_LEVEL_INFO:
            levelName = "INFO ";
            color = COLOR_INFO;
            break;
        case LOG_LEVEL_WARN:
            levelName = "WARN ";
            color = COLOR_WARN;
            break;
        case LOG_LEVEL_ERROR:
            levelName = "ERROR";
            color = COLOR_ERROR;
            break;
        default:
            levelName = "?????";
            color = COLOR_DEFAULT;
    }

    std::ostringstream logMessage;
    logMessage << "[" << timestamp << "][" << levelName << "][" << GetComponentName(component) << "] " << msg;

    SetConsoleColor(color);
    std::cout << logMessage.str() << std::endl;
    SetConsoleColor(COLOR_DEFAULT);

    if (LOG_TO_FILE && s_LogFile.is_open()) {
        s_LogFile << logMessage.str() << std::endl;
        s_LogFile.flush();
    }
}

const char* Logger::GetComponentName(int component) {
    switch (component) {
        case LOG_COMPONENT_NETWORK: return "NETWORK";
        case LOG_COMPONENT_CLIENTAPP: return "APP";
        case LOG_COMPONENT_FARMBOT: return "FARMBOT";
        case LOG_COMPONENT_CONSOLE: return "CONSOLE";
        case LOG_COMPONENT_FEATURES: return "FEATURES";
        default: return "UNKNOWN";
    }
}

void Logger::Cleanup() {
    if (s_LogFile.is_open()) {
        s_LogFile.close();
    }

    if (s_ConsoleHandle != INVALID_HANDLE_VALUE) {
        s_ConsoleHandle = INVALID_HANDLE_VALUE;
    }

    if (s_Initialized && LOG_CONSOLE) {
        FreeConsole();
        s_Initialized = false;
    }
}

}