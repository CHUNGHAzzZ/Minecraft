#pragma once

#include <chrono>

namespace Minecraft {

class Time {
public:
    static void Init();
    static void Update();
    
    static float DeltaTime() { return s_DeltaTime; }
    static float TotalTime() { return s_TotalTime; }
    static int FPS() { return s_FPS; }

private:
    static std::chrono::high_resolution_clock::time_point s_LastFrame;
    static std::chrono::high_resolution_clock::time_point s_StartTime;
    static float s_DeltaTime;
    static float s_TotalTime;
    static int s_FPS;
    static int s_FrameCount;
    static float s_FPSTimer;
};

} // namespace Minecraft
