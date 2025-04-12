#pragma once
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <fstream>
#include <mutex>

namespace Common {

// Log levels
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_NONE  4

// Component masks for logging
#define LOG_COMPONENT_NETWORK  (1 << 0)
#define LOG_COMPONENT_CLIENTAPP (1 << 1)
#define LOG_COMPONENT_FARMBOT  (1 << 2)
#define LOG_COMPONENT_CONSOLE  (1 << 3)
#define LOG_COMPONENT_FEATURES (1 << 4)

// Console colors (Windows)
#define COLOR_DEFAULT 7
#define COLOR_DEBUG   8   // Gray
#define COLOR_INFO    7   // White
#define COLOR_WARN    14  // Yellow
#define COLOR_ERROR   12  // Red

// Default configuration - can be overridden
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_ERROR  // Show all log levels including debug
#endif

#ifndef LOG_CONSOLE
#define LOG_CONSOLE 1  // Enable console by default
#endif

#ifndef LOG_COMPONENTS
#define LOG_COMPONENTS (LOG_COMPONENT_CLIENTAPP | LOG_COMPONENT_FARMBOT | LOG_COMPONENT_CONSOLE  | LOG_COMPONENT_FEATURES | LOG_COMPONENT_NETWORK)
#endif

#ifndef LOG_TO_FILE
#define LOG_TO_FILE true
#endif

class Logger {
private:
    static std::mutex s_Mutex;
    static std::ofstream s_LogFile;
    static bool s_Initialized;
    static HANDLE s_ConsoleHandle;

    static void Initialize();
    static void SetConsoleColor(int color);

public:
    static void Log(int level, int component, const std::string& msg);
    static const char* GetComponentName(int component);
    static void Cleanup();
};

// Logging macros
#define LOG_DEBUG(component, msg) \
    if (LOG_LEVEL <= LOG_LEVEL_DEBUG && (LOG_COMPONENTS & component)) { \
        std::ostringstream ss; ss << msg; \
        Common::Logger::Log(LOG_LEVEL_DEBUG, component, ss.str()); \
    }

#define LOG_INFO(component, msg) \
    if (LOG_LEVEL <= LOG_LEVEL_INFO && (LOG_COMPONENTS & component)) { \
        std::ostringstream ss; ss << msg; \
        Common::Logger::Log(LOG_LEVEL_INFO, component, ss.str()); \
    }

#define LOG_WARN(component, msg) \
    if (LOG_LEVEL <= LOG_LEVEL_WARN && (LOG_COMPONENTS & component)) { \
        std::ostringstream ss; ss << msg; \
        Common::Logger::Log(LOG_LEVEL_WARN, component, ss.str()); \
    }

#define LOG_ERROR(component, msg) \
    if (LOG_LEVEL <= LOG_LEVEL_ERROR && (LOG_COMPONENTS & component)) { \
        std::ostringstream ss; ss << msg; \
        Common::Logger::Log(LOG_LEVEL_ERROR, component, ss.str()); \
    }

} // namespace Common