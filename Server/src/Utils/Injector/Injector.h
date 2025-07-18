#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include "ManualMap.h"

namespace FracqServer {

class Injector {
public:
    Injector() = default;
    ~Injector() = default;

    bool InjectByProcessName(const std::string& processName, const std::string& dllPath);
    bool InjectByPID(DWORD pid, const std::string& dllPath);
    bool InjectByPIDLoadLibrary(DWORD pid, const std::string& dllPath);
    bool InjectByProcessNameLoadLibrary(const std::string& processName, const std::string& dllPath);

    std::string GetLastError() const { return m_LastError; }

private:
    bool IsCorrectTargetArchitecture(HANDLE hProc);
    DWORD GetProcessIdByName(const std::string& processName);
    bool ValidateAndPrepareInjection(HANDLE hProc, const std::string& dllPath);
    bool EnableDebugPrivilege();
    bool LoadAndInjectDll(HANDLE hProc, const std::string& dllPath);
    std::wstring StringToWString(const std::string& str);

    std::string m_LastError;
};

} // namespace FracqServer