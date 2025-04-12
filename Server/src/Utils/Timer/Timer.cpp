#include "Timer.h"
#include <chrono>

namespace Utils {

double Timer::GetTime()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = currentTime - startTime;
    return elapsed.count();
}

}