#include "GameFunctions.h"
#include "hack/globals/Globals.h"

namespace FracqClient {

void GameFunctions::SendAttackPacket(uint32_t vid) {
    uintptr_t netptr = Globals::Get()->PythonNetworkStream;
    uintptr_t attackcall = Globals::Get()->SendAttackPacket;

    __try {
        __asm {
            mov ecx, netptr
            push vid
            push 0
            call attackcall
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // Handle exception silently
    }
}

void GameFunctions::SendCharacterStatePacket(Math::Vector3 pos,
                                             float rot,
                                             uint32_t eFunc,
                                             uint32_t uArg) {
    uintptr_t sendstatecall = Globals::Get()->SendCharacterStatePacket;
    uintptr_t netptr = Globals::Get()->PythonNetworkStream;
    Math::Vector3 _pos = pos * 100.0f;

    __try {
        __asm {
            mov ecx, netptr
            push uArg
            push eFunc
            push rot
            lea eax, _pos
            push eax
            call sendstatecall
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // Handle exception silently
    }
}

void GameFunctions::SendClickItemPacket(uint32_t vid) {
    uintptr_t pythonplayer = Globals::Get()->PythonPlayer;
    uintptr_t clickitemcall = Globals::Get()->SendClickItemPacket;

    __try {
        __asm {
            mov ecx, pythonplayer
            push vid
            call clickitemcall
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        // Handle exception silently
    }
}

} // namespace FracqClient
