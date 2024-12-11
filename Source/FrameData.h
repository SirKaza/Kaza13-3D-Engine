#pragma once
#include <chrono>
#include <stddef.h>
#include <vector>

extern std::vector<float> fps_log;
extern std::vector<float> ms_log;
const size_t max_log_size = 100;
extern std::chrono::steady_clock::time_point startTime;
extern int maxFPS;

void updateFrameData();
void addFrameData(float fps, float ms);