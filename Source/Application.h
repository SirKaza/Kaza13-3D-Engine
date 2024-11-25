#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleOpenGL;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleProgram;
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
    ModuleProgram* GetProgram() { return program; }
    ModuleRender* GetRender() { return render; }
    ModuleDebugDraw* GetDebugDraw() { return debugDraw; }
    ModuleEditor* GetEditor() { return editor; }

private:

    ModuleOpenGL* opengl = nullptr;
    ModuleWindow* window = nullptr;
    ModuleInput* input = nullptr;
    ModuleProgram* program = nullptr;
    ModuleRender* render = nullptr;
    ModuleDebugDraw* debugDraw = nullptr;
    ModuleEditor* editor = nullptr;

    std::list<Module*> modules;

};

extern Application* App;
