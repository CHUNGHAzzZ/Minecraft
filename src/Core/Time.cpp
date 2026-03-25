#include "Time.h"

namespace Minecraft {

std::chrono::high_resolution_clock::time_point Time::s_LastFrame;
std::chrono::high_resolution_clock::time_point Time::s_StartTime;
float Time::s_DeltaTime = 0.0f;
float Time::s_TotalTime = 0.0f;
int Time::s_FPS = 0;
int Time::s_FrameCount = 0;
float Time::s_FPSTimer = 0.0f;

void Time::Init() {
    s_StartTime = std::chrono::high_resolution_clock::now();
    s_LastFrame = s_StartTime;
}

void Time::Update() {
    auto currentFrame = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = currentFrame - s_LastFrame;
    s_DeltaTime = delta.count();
    s_LastFrame = currentFrame;
    
    std::chrono::duration<float> total = currentFrame - s_StartTime;
    s_TotalTime = total.count();
    
    // FPS calculation
    s_FrameCount++;
    s_FPSTimer += s_DeltaTime;
    if (s_FPSTimer >= 1.0f) {
        s_FPS = s_FrameCount;
        s_FrameCount = 0;
        s_FPSTimer = 0.0f;
    }
}

} // namespace Minecraft
