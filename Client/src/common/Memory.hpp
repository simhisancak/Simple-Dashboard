#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <cstdint>
#include <string>
#include <vector>

namespace Common {

class Memory {
private:
    static bool PatternCompare(const uint8_t* data, const uint8_t* pattern, const char* mask) {
        for (; *mask; ++mask, ++data, ++pattern) {
            if (*mask == 'x' && *data != *pattern)
                return false;
        }
        return true;
    }

    static bool
    ConvertPattern(const char* pattern, std::vector<uint8_t>& bytes, std::string& mask) {
        bytes.clear();
        mask.clear();

        for (const char* ptr = pattern; *ptr; ++ptr) {
            if (*ptr == ' ' || *ptr == '\t')
                continue;

            if (*ptr == '?') {
                bytes.push_back(0);
                mask += '?';
                continue;
            }

            if (!isxdigit(ptr[0]) || !isxdigit(ptr[1]))
                return false;

            uint8_t byte = (uint8_t)strtoul(ptr, nullptr, 16);
            bytes.push_back(byte);
            mask += 'x';
            ptr++;
        }
        return true;
    }

    static bool ConvertHexString(const char* pattern, std::vector<uint8_t>& bytes) {
        bytes.clear();

        for (const char* ptr = pattern; *ptr; ptr += 2) {
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;

            if (!*ptr)
                break;

            if (!isxdigit(ptr[0]) || !isxdigit(ptr[1]))
                return false;

            uint8_t byte = (uint8_t)strtoul(ptr, nullptr, 16);
            bytes.push_back(byte);
        }
        return true;
    }

public:
    template <typename T> static T Read(uintptr_t address) {
        if (!IsValidPtr(address))
            return T();

        __try {
            return *reinterpret_cast<T*>(address);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return T();
        }
    }

    template <typename T> static void Write(uintptr_t address, const T& value) {
        if (!IsValidPtr(address))
            return;

        __try {
            *reinterpret_cast<T*>(address) = value;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            // Write failed silently
        }
    }

    static bool IsValidPtr(uintptr_t ptr) { return ptr != 0 && ptr > 0x10000; }

    template <typename T> static bool IsNullPtr(T* ptr) { return ptr == nullptr; }

    // Offset helpers
    template <typename T> static T GetOffset(uintptr_t base, uintptr_t offset) {
        if (!IsValidPtr(base))
            return T();

        __try {
            return Read<T>(base + offset);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return T();
        }
    }

    // Pointer chain resolver
    template <typename T>
    static T ReadChain(uintptr_t base, std::initializer_list<uintptr_t> offsets) {
        if (!IsValidPtr(base))
            return T();

        __try {
            uintptr_t current = base;
            for (auto offset : offsets) {
                if (!IsValidPtr(current))
                    return T();
                current = Read<uintptr_t>(current + offset);
            }
            return Read<T>(current);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return T();
        }
    }

    // VTable helpers
    static uintptr_t GetVTableFunction(uintptr_t instance, int index) {
        if (!IsValidPtr(instance))
            return 0;

        __try {
            uintptr_t vtable = Read<uintptr_t>(instance);
            return Read<uintptr_t>(vtable + (index * sizeof(void*)));
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return 0;
        }
    }

    // Memory protection utilities
    static bool Protect(uintptr_t address, size_t size, DWORD protection, DWORD* oldProtection) {
        if (!IsValidPtr(address))
            return false;

        __try {
            return VirtualProtect(reinterpret_cast<void*>(address),
                                  size,
                                  protection,
                                  oldProtection);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    // Patch bytes
    static bool PatchBytes(uintptr_t address, const BYTE* bytes, size_t size) {
        if (!IsValidPtr(address) || !bytes)
            return false;

        __try {
            DWORD oldProtection;
            if (!Protect(address, size, PAGE_EXECUTE_READWRITE, &oldProtection))
                return false;

            memcpy(reinterpret_cast<void*>(address), bytes, size);

            DWORD temp;
            return Protect(address, size, oldProtection, &temp);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    // NOP instruction patch
    static bool NOP(uintptr_t address, size_t size) {
        std::vector<BYTE> nops(size, 0x90);
        return PatchBytes(address, nops.data(), size);
    }

    // IDA style pattern scan in current module
    static uintptr_t FindPattern(const char* pattern) { return FindPattern(nullptr, pattern); }

    // IDA style pattern scan in specific module
    static uintptr_t FindPattern(const char* moduleName, const char* pattern) {
        std::vector<uint8_t> bytes;
        std::string mask;

        if (!ConvertPattern(pattern, bytes, mask))
            return 0;

        MODULEINFO modInfo;
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (!hModule)
            return 0;

        GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));

        uint8_t* begin = static_cast<uint8_t*>(modInfo.lpBaseOfDll);
        uint8_t* end = begin + modInfo.SizeOfImage - bytes.size();

        for (uint8_t* curr = begin; curr < end; ++curr) {
            if (PatternCompare(curr, bytes.data(), mask.c_str())) {
                return reinterpret_cast<uintptr_t>(curr);
            }
        }
        return 0;
    }

    // Pattern scan with explicit pattern and mask in current module
    static uintptr_t FindPatternMask(const char* pattern, const char* mask) {
        return FindPatternMask(nullptr, pattern, mask);
    }

    // Pattern scan with explicit pattern and mask in specific module
    static uintptr_t
    FindPatternMask(const char* moduleName, const char* pattern, const char* mask) {
        std::vector<uint8_t> bytes;
        if (!ConvertHexString(pattern, bytes))
            return 0;

        MODULEINFO modInfo;
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (!hModule)
            return 0;

        GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));

        uint8_t* begin = static_cast<uint8_t*>(modInfo.lpBaseOfDll);
        uint8_t* end = begin + modInfo.SizeOfImage - strlen(mask);

        for (uint8_t* curr = begin; curr < end; ++curr) {
            if (PatternCompare(curr, bytes.data(), mask)) {
                return reinterpret_cast<uintptr_t>(curr);
            }
        }
        return 0;
    }

    // Find all pattern matches in current module
    static std::vector<uintptr_t> FindPatternAll(const char* pattern) {
        return FindPatternAll(nullptr, pattern);
    }

    // Find all pattern matches in specific module
    static std::vector<uintptr_t> FindPatternAll(const char* moduleName, const char* pattern) {
        std::vector<uintptr_t> results;
        std::vector<uint8_t> bytes;
        std::string mask;

        if (!ConvertPattern(pattern, bytes, mask))
            return results;

        MODULEINFO modInfo;
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (!hModule)
            return results;

        GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));

        uint8_t* begin = static_cast<uint8_t*>(modInfo.lpBaseOfDll);
        uint8_t* end = begin + modInfo.SizeOfImage - bytes.size();

        for (uint8_t* curr = begin; curr < end; ++curr) {
            if (PatternCompare(curr, bytes.data(), mask.c_str())) {
                results.push_back(reinterpret_cast<uintptr_t>(curr));
            }
        }
        return results;
    }

    // Find all pattern matches with explicit pattern and mask in current module
    static std::vector<uintptr_t> FindPatternMaskAll(const char* pattern, const char* mask) {
        return FindPatternMaskAll(nullptr, pattern, mask);
    }

    // Find all pattern matches with explicit pattern and mask in specific module
    static std::vector<uintptr_t>
    FindPatternMaskAll(const char* moduleName, const char* pattern, const char* mask) {
        std::vector<uintptr_t> results;
        std::vector<uint8_t> bytes;
        if (!ConvertHexString(pattern, bytes))
            return results;

        MODULEINFO modInfo;
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (!hModule)
            return results;

        GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));

        uint8_t* begin = static_cast<uint8_t*>(modInfo.lpBaseOfDll);
        uint8_t* end = begin + modInfo.SizeOfImage - strlen(mask);

        for (uint8_t* curr = begin; curr < end; ++curr) {
            if (PatternCompare(curr, bytes.data(), mask)) {
                results.push_back(reinterpret_cast<uintptr_t>(curr));
            }
        }
        return results;
    }

    // Resolve a relative call/jmp address (E8/E9 style)
    static uintptr_t ResolveCall(uintptr_t address) {
        if (!IsValidPtr(address))
            return 0;

        __try {
            return *(int32_t*)(address + 1) + address + 5;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return 0;
        }
    }

    // Resolve a pointer reference (like mov instruction)
    static uintptr_t ResolvePointer(uintptr_t address, uint32_t offset = 0x2) {
        if (!IsValidPtr(address))
            return 0;

        __try {
            return *(uint32_t*)(address + offset);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return 0;
        }
    }

    // Pattern scan and resolve call in one go
    static uintptr_t FindAndResolveCall(const char* moduleName = nullptr,
                                        const char* pattern = nullptr) {
        uintptr_t address = FindPattern(moduleName, pattern);
        if (!address)
            return 0;
        return ResolveCall(address);
    }

    // Pattern scan and resolve pointer in one go
    static uintptr_t FindAndResolvePointer(const char* moduleName = nullptr,
                                           const char* pattern = nullptr,
                                           uint32_t offset = 0x2) {
        uintptr_t address = FindPattern(moduleName, pattern);
        if (!address)
            return 0;
        return ResolvePointer(address, offset);
    }

    // Resolve multiple pointer levels (pointer chain)
    template <typename T = uintptr_t>
    static T ResolveMultiLevel(uintptr_t base, std::initializer_list<uint32_t> offsets) {
        if (!IsValidPtr(base))
            return T();

        __try {
            uintptr_t current = base;
            for (auto offset : offsets) {
                if (!IsValidPtr(current))
                    return T();
                current = *(uintptr_t*)(current + offset);
            }
            return (T)current;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return T();
        }
    }

    // Pattern scan and resolve multi-level pointer in one go
    template <typename T = uintptr_t>
    static T FindAndResolveMultiLevel(const char* moduleName = nullptr,
                                      const char* pattern = nullptr,
                                      std::initializer_list<uint32_t> offsets = {}) {
        uintptr_t address = FindPattern(moduleName, pattern);
        if (!address)
            return T();
        return ResolveMultiLevel<T>(address, offsets);
    }
};

} // namespace Common
