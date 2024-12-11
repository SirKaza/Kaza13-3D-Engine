#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "SDL/include/SDL.h"
#include <imgui_impl_sdl2.h>
#include "Math/float3.h"

ModuleInput::ModuleInput()
{}

// Destructor
ModuleInput::~ModuleInput()
{}

// Called before render is available
bool ModuleInput::Init()
{
	ENGINE_LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		ENGINE_LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::Update()
{
    SDL_Event sdlEvent;
    mouseMotionX = mouseMotionY = mouseWheelY = 0; // reset motion each frame

    while (SDL_PollEvent(&sdlEvent) != 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                return UPDATE_STOP;

            case SDL_WINDOWEVENT:
                if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
                    App->GetOpenGL()->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2, sdlEvent.window.windowID);
                break;
                
            case SDL_MOUSEMOTION:
                mouseX = sdlEvent.motion.x;
                mouseY = sdlEvent.motion.y;
                mouseMotionX = sdlEvent.motion.xrel; 
                mouseMotionY = sdlEvent.motion.yrel;
                break;

            case SDL_MOUSEWHEEL:
                mouseWheelY = sdlEvent.wheel.y;
                break;

            case SDL_DROPFILE:
                const char* file = sdlEvent.drop.file;

                if (strstr(file, ".gltf") != nullptr)
                    App->GetRender()->setModel(file);
                else if (strstr(file, ".png") != nullptr || strstr(file, ".dds") != nullptr)
                    App->GetRender()->setModel(file);

                SDL_free(sdlEvent.drop.file);
                break;
        }
    }

    keyboard = SDL_GetKeyboardState(NULL); // should not be freed by the caller. SDL manages it
    mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	ENGINE_LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
