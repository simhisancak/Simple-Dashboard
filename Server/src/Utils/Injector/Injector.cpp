#include "Injector.h"
#include <fstream>
#include <iostream>

Injector::Injector() {}

Injector::~Injector() {}

std::wstring Injector::StringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(size_needed - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    return wstr;
}

bool Injector::InjectByProcessName(const std::string& processName, const std::string& dllPath) {
    DWORD pid = GetProcessIdByName(processName);
    if (pid == 0) {
        m_LastError = "Process not found";
        return false;
    }
    return InjectByPID(pid, dllPath);
}

bool Injector::InjectByPID(DWORD pid, const std::string& dllPath) {
    std::cout << "Process PID: " << pid << std::endl;

    if (!EnableDebugPrivilege()) {
        m_LastError = "Failed to enable debug privilege";
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProc) {
        m_LastError = "Failed to open process";
        return false;
    }

    bool result = ValidateAndPrepareInjection(hProc, dllPath);
    if (!result) {
        CloseHandle(hProc);
        return false;
    }

    result = LoadAndInjectDll(hProc, dllPath);
    CloseHandle(hProc);
    
    return result;
}

bool Injector::IsCorrectTargetArchitecture(HANDLE hProc) {
    BOOL bTarget = FALSE;
    if (!IsWow64Process(hProc, &bTarget)) {
        m_LastError = "Can't confirm target process architecture";
        return false;
    }

    BOOL bHost = FALSE;
    IsWow64Process(GetCurrentProcess(), &bHost);

    return (bTarget == bHost);
}

DWORD Injector::GetProcessIdByName(const std::string& processName) {
    std::wstring wProcessName = StringToWString(processName);
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (snapshot == INVALID_HANDLE_VALUE) {
        m_LastError = "Failed to create process snapshot";
        return 0;
    }

    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, wProcessName.c_str()) == 0) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    m_LastError = "Process not found";
    return 0;
}

bool Injector::ValidateAndPrepareInjection(HANDLE hProc, const std::string& dllPath) {
    if (!IsCorrectTargetArchitecture(hProc)) {
        m_LastError = "Invalid Process Architecture";
        return false;
    }

    std::wstring wDllPath = StringToWString(dllPath);
    if (GetFileAttributesW(wDllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        m_LastError = "DLL file doesn't exist";
        return false;
    }

    return true;
}

bool Injector::EnableDebugPrivilege() {
    TOKEN_PRIVILEGES priv = { 0 };
    HANDLE hToken = NULL;
    
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        priv.PrivilegeCount = 1;
        priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid)) {
            return AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL);
        }
        CloseHandle(hToken);
    }
    m_LastError = "Failed to enable debug privilege";
    return false;
}

bool Injector::LoadAndInjectDll(HANDLE hProc, const std::string& dllPath) {
    std::wstring wDllPath = StringToWString(dllPath);
    std::ifstream File(wDllPath, std::ios::binary | std::ios::ate);

    if (File.fail()) {
        m_LastError = "Failed to open DLL file";
        return false;
    }

    auto FileSize = File.tellg();
    if (FileSize < 0x1000) {
        m_LastError = "Invalid DLL file size";
        File.close();
        return false;
    }

    SIZE_T size = static_cast<SIZE_T>(FileSize);
    BYTE* pSrcData = new BYTE[size];
    if (!pSrcData) {
        m_LastError = "Failed to allocate memory for DLL";
        File.close();
        return false;
    }

    File.seekg(0, std::ios::beg);
    File.read((char*)(pSrcData), size);
    File.close();

    std::cout << "Mapping...\n";
    bool result = ManualMapDll(hProc, pSrcData, size);
    delete[] pSrcData;

    if (!result) {
        m_LastError = "Manual mapping failed";
        return false;
    }

    return true;
} 