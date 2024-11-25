#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
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
        }
    }

    keyboard = SDL_GetKeyboardState(NULL);
    float speed = 2.0f;
    if (keyboard[SDL_SCANCODE_LSHIFT])
        speed *= 3;
    bool moved = false;
    float3 newPos;
    ModuleCamera* camera = App->GetCamera();
    Frustum frustum = camera->getFrustum();
    float deltaTime = App->GetDeltaTime();

    if ((keyboard[SDL_SCANCODE_Q] || keyboard[SDL_SCANCODE_E]) && !(keyboard[SDL_SCANCODE_Q] && keyboard[SDL_SCANCODE_E])) // Q up E down
    {
        float3 moveDirection = frustum.up.Normalized();  // normalize to evade irregular movements
        if (keyboard[SDL_SCANCODE_Q])
        {
            newPos = frustum.pos - moveDirection * speed * deltaTime;
        }
        else if (keyboard[SDL_SCANCODE_E])
        {
            newPos = frustum.pos + moveDirection * speed * deltaTime;
        }
        camera->setPosition(newPos);
        moved = true;
    }

    if ((keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_S]) && !(keyboard[SDL_SCANCODE_W] && keyboard[SDL_SCANCODE_S])) // W forward S backwards
    {
        float3 moveDirection = frustum.front.Normalized();  // normalize to evade irregular movements

        if (keyboard[SDL_SCANCODE_W]) 
        {
            newPos = frustum.pos + moveDirection * speed * deltaTime;
            
        }
        else if (keyboard[SDL_SCANCODE_S]) 
        {
            newPos = frustum.pos - moveDirection * speed * deltaTime;
        }
        camera->setPosition(newPos);
        moved = true;
    }

    if ((keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_D]) && !(keyboard[SDL_SCANCODE_A] && keyboard[SDL_SCANCODE_D])) // A left D right
    {
        float3 moveDirection = frustum.WorldRight().Normalized();  // normalize to evade irregular movements

        if (keyboard[SDL_SCANCODE_A]) 
        {
            newPos = frustum.pos - moveDirection * speed * deltaTime;
        }
        else if (keyboard[SDL_SCANCODE_D]) 
        {
            newPos = frustum.pos + moveDirection * speed * deltaTime;
        }
        camera->setPosition(newPos);
        moved = true;
    }
    


    if (moved) {
        ENGINE_LOG("New camera position: (%.2f, %.2f, %.2f)", frustum.pos.x, frustum.pos.y, frustum.pos.z);
        ENGINE_LOG("Front: (%.2f, %.2f, %.2f)", frustum.front.x, frustum.front.y, frustum.front.z);
        ENGINE_LOG("Up: (%.2f, %.2f, %.2f)", frustum.up.x, frustum.up.y, frustum.up.z);
    }

    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	ENGINE_LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
