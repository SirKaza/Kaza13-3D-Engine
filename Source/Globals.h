#pragma once
#define NOMINMAX
#include <windows.h>
#include <stdio.h>

#define ENGINE_LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Super Awesome Engine"
#define GLSL_VERSION "#version 460"
#define ENGINE_VERSION "0.1.0"
#define ENGINE_NAME "Kaza13"
#define ORGANIZATION_NAME "UPC TALENT"