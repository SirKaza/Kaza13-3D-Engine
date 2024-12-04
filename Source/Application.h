#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"
#include <chrono>

class ModuleOpenGL;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleTexture;
class ModuleProgram;
class ModuleCamera;
class ModuleRender;
class ModuleDebugDraw;
class ModuleEditor;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

    ModuleOpenGL* GetOpenGL() { return opengl; }
    ModuleWindow* GetWindow() { return window; }
    ModuleInput*  GetInput() { return input; }
    ModuleTexture* GetTexture() { return texture; }
    ModuleProgram* GetProgram() { return program; }
    ModuleCamera* GetCamera() { return camera; }
    ModuleRender* GetRender() { return render; }
    ModuleDebugDraw* GetDebugDraw() { return debugDraw; }
    ModuleEditor* GetEditor() { return editor; }

    float GetDeltaTime() {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = now - lastTime;
        lastTime = now;
        return deltaTime.count(); 
    }

private:

    ModuleOpenGL* opengl = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleTexture* texture = nullptr;
    ModuleProgram* program = nullptr;
    ModuleCamera* camera = nullptr;
    ModuleRender* render = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
    ModuleEditor* editor = nullptr;

    std::list<Module*> modules;

    std::chrono::high_resolution_clock::time_point lastTime;
};

extern Application* App;
