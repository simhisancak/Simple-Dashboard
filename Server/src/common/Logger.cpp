#include "Logger.h"

namespace Common {

// Static member initialization
std::mutex Logger::s_Mutex;
std::ofstream Logger::s_LogFile;
bool Logger::s_Initialized = false;
HANDLE Logger::s_ConsoleHandle = INVALID_HANDLE_VALUE;

void Logger::Initialize() {
    if (!s_Initialized) {
        s_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (LOG_TO_FILE) {
            // Get timestamp for filename
            auto now = std::time(nullptr);
            auto tm = *std::localtime(&now);
            char filename[64];
            strftime(filename, sizeof(filename), "server_%Y%m%d_%H%M%S.log", &tm);
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

    // Get timestamp
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm);

    // Get level name and color
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

    // Format message
    std::ostringstream logMessage;
    logMessage << "[" << timestamp << "][" << levelName << "][" << GetComponentName(component) << "] " << msg;

    // Output to console with color
    SetConsoleColor(color);
    std::cout << logMessage.str() << std::endl;
    SetConsoleColor(COLOR_DEFAULT);

    // Output to file if enabled
    if (LOG_TO_FILE && s_LogFile.is_open()) {
        s_LogFile << logMessage.str() << std::endl;
        s_LogFile.flush();
    }
}

const char* Logger::GetComponentName(int component) {
    switch (component) {
        case LOG_COMPONENT_NETWORK: return "NETWORK";
        case LOG_COMPONENT_SERVER: return "SERVER";
        case LOG_COMPONENT_CONSOLE: return "CONSOLE";
        default: return "UNKNOWN";
    }
}

void Logger::Cleanup() {
    if (s_LogFile.is_open()) {
        s_LogFile.close();
    }
}

void Logger::EnableFileLogging(bool enable) {
    std::lock_guard<std::mutex> lock(s_Mutex);
    
    if (enable && !s_LogFile.is_open()) {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        char filename[64];
        strftime(filename, sizeof(filename), "server_%Y%m%d_%H%M%S.log", &tm);
        s_LogFile.open(filename, std::ios::out | std::ios::app);
    }
    else if (!enable && s_LogFile.is_open()) {
        s_LogFile.close();
    }
}

} // namespace Common 