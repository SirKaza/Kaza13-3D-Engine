#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleEditor.h"
#include "EditorMenu.h"

ModuleWindow::ModuleWindow()
{
	SDL_zero(displayMode);
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	ENGINE_LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ENGINE_LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = SCREEN_WIDTH;
		int height = SCREEN_HEIGHT;

		if (SDL_GetDesktopDisplayMode(0, &displayMode) == 0) // success
		{
			// set window in relation to desktop size
			width = static_cast<int>(displayMode.w * 0.4);
			height = static_cast<int>(displayMode.h * 0.4);
		}

		Uint32 flags = SDL_WINDOW_SHOWN |  SDL_WINDOW_OPENGL;

		if(FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
			initWindowFlags.fullscreen = true;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			ENGINE_LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			if (VSYNC == true)
			{
				SDL_GL_SetSwapInterval(1);
				initWindowFlags.vsync = true;
			}
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	ENGINE_LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::setFullscreen(bool fullscreen)
{
	if (fullscreen) {
		SDL_SetWindowDisplayMode(window, &displayMode);
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		ENGINE_LOG("Fullscreen enabled");
	}
	else {
		SDL_SetWindowFullscreen(window, 0);
		screen_surface = SDL_GetWindowSurface(window);
		App->GetOpenGL()->WindowResized(screen_surface->w, screen_surface->h, SDL_GetWindowID(window));
		ENGINE_LOG("Fullscreen disabled");
	}
}

void ModuleWindow::setBorderless(bool borderless)
{
	SDL_bool sdl_borderless = borderless ? SDL_FALSE : SDL_TRUE;
	SDL_SetWindowBordered(window, sdl_borderless);
	if (borderless)
	{
		ENGINE_LOG("Borderless enabled");
	}
	else
	{
		ENGINE_LOG("Borderless disabled");
	}
}

void ModuleWindow::setResizable(bool resizable)
{
	SDL_bool sdl_resizable = resizable ? SDL_TRUE : SDL_FALSE;
	SDL_SetWindowResizable(window, sdl_resizable);
	if (resizable)
	{
		ENGINE_LOG("Resizable enabled");
	}
	else
	{
		ENGINE_LOG("Resizable disabled");
	}
}

void ModuleWindow::setFullDesktop(bool fullDesktop)
{
	if (fullDesktop)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		ENGINE_LOG("Fullscreen desktop enabled");
	}
	else {
		SDL_SetWindowFullscreen(window, 0);
		screen_surface = SDL_GetWindowSurface(window);
		App->GetOpenGL()->WindowResized(screen_surface->w, screen_surface->h, SDL_GetWindowID(window));
		ENGINE_LOG("Fullscreen desktop disabled");
	}
}

void ModuleWindow::setVsync(bool vsync)
{
	if (vsync)
	{
		SDL_GL_SetSwapInterval(1);
		ENGINE_LOG("Vsync enabled");
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
		ENGINE_LOG("Vsync disabled");
	}
}

void ModuleWindow::setBrightness(float brightness)
{
	SDL_SetWindowBrightness(window, brightness);
	ENGINE_LOG("Brightness set to %.3f", brightness);
}

float ModuleWindow::getBrightness() const
{
	return SDL_GetWindowBrightness(window);
}
