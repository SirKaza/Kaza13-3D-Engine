#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"
#include "EditorMenu.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow();

	// Called before quitting
	bool Init();

	// Called before quitting
	bool CleanUp();

	void setFullscreen(bool fullscreen);
	void setBorderless(bool borderless);
	void setResizable(bool resizable);
	void setFullDesktop(bool fullDesktop);
	void setVsync(bool vsync);
	int getRefreshRate();

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen_surface = NULL;

	SDL_DisplayMode displayMode;
	WindowFlags lastFlags;
};

#endif // __ModuleWindow_H__