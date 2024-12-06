#include "FrameData.h"
#include <chrono>
#include <stddef.h>
#include <vector>
#include <thread> // std::this_thread::sleep_for

std::vector<float> fps_log;
std::vector<float> ms_log;
std::chrono::steady_clock::time_point startTime;
int maxFPS = 60;

// Improvements: SDL_Delay() & VSYNC
void UpdateFrameData() {
    float target_frame_time = 1000.0f / maxFPS; // 5Hz = 200ms

    // Calculate Frame time
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> delta = endTime - startTime;

    if (maxFPS != 0) {
        if (delta.count() < target_frame_time) {
            double wait_time = target_frame_time - delta.count();
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((target_frame_time - delta.count()))));
            endTime = std::chrono::steady_clock::now();
            delta = endTime - startTime;
        }
    }

    startTime = endTime;

    float frame_time = static_cast<float>(delta.count());
    float fps =  1000.0f / frame_time; // Calculate FPS
    float ms = frame_time;

    // Add FPS & MS to LOG
    AddFrameData(fps, ms);
}

void AddFrameData(float fps, float ms) {
    if (fps_log.size() >= max_log_size) {
        fps_log.erase(fps_log.begin()); 
    }
    if (ms_log.size() >= max_log_size) {
        ms_log.erase(ms_log.begin()); 
    }

    fps_log.push_back(fps);
    ms_log.push_back(ms);
}