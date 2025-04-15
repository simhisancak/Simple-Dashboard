#pragma once
#include "features/Farmbot.h"
#include "../instance/Instance.h"
#include "common/Logger.h"
#include "Instance.h"
#include <map>
#include <Windows.h>
#include <winternl.h>
#include <d3d9.h>

using TCharacterInstanceMap = std::map<uint32_t, uintptr_t>;

class Helper
{
public:
    static bool CompareInstances(const Instance &a, const Instance &b);
    static bool ComparePacketsInstances(const Packets::Instance &a, const Packets::Instance &b);
    static Instance GetMainActor();
    static std::vector<Instance> getMobList(MobType targetTypes);
    static std::vector<Packets::Instance> getMobs(MobType targetTypes);
    static void setTargetVid(uint32_t vid);
    static TCharacterInstanceMap getAlivaInstMap();
    static void RenderCondition(bool enable);
    static bool ClearRam();

private:
    #define ProcessQuotaLimits 1

    typedef struct _PROCESS_QUOTA_LIMITS {
        SIZE_T MinimumWorkingSetSize;
        SIZE_T MaximumWorkingSetSize;
        SIZE_T PagedPoolLimit;
        SIZE_T NonPagedPoolLimit;
        SIZE_T TimeLimit;
    } PROCESS_QUOTA_LIMITS, *PPROCESS_QUOTA_LIMITS;

    typedef NTSTATUS (NTAPI *pNtSetInformationProcess)(
        IN HANDLE ProcessHandle,
        IN ULONG ProcessInformationClass,
        IN PVOID ProcessInformation,
        IN ULONG ProcessInformationLength
    );
};