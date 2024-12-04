#pragma once
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include "Math/MathNamespace.h"
#include "Math/MathConstants.h"

#define ENGINE_LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

struct WindowFlags
{
    bool fullscreen = false;
    bool borderless = false;
    bool resizable = false;
    bool fullDesktop = false;
    bool vsync = false;

    bool operator!=(const WindowFlags& other) const {
        return fullscreen != other.fullscreen || borderless != other.borderless || resizable != other.resizable || fullDesktop != other.fullDesktop || vsync != other.vsync;
    }
};

// Conversions
#define RadToDeg(x) ((x) * (180.0f / math::pi))
#define DegToRad(x) ((x) * (math::pi / 180.0f))

// Camera Config
#define CAMERA_SPEED 3.0f
#define ROTATE_SENSITIVITY 0.1f
#define DRAG_SENSITIVITY 0.01f
#define ZOOM_SPEED 0.05f
#define WHEEL_SPEED 0.5f

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