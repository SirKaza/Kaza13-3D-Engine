#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "SDL.h"
#include <GL/glew.h>

void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* tmp_source = "", * tmp_type = "", * tmp_severity = "";
	switch (source) {
		case GL_DEBUG_SOURCE_API: tmp_source = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmp_source = "Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: tmp_source = "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: tmp_source = "Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: tmp_source = "Application"; break;
		case GL_DEBUG_SOURCE_OTHER: tmp_source = "Other"; break;
	};
	switch (type) {
		case GL_DEBUG_TYPE_ERROR: tmp_type = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmp_type = "Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmp_type = "Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY: tmp_type = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: tmp_type = "Performance"; break;
		case GL_DEBUG_TYPE_MARKER: tmp_type = "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: tmp_type = "Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP: tmp_type = "Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER: tmp_type = "Other"; break;
	};
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: tmp_severity = "high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: tmp_severity = "medium"; break;
		case GL_DEBUG_SEVERITY_LOW: tmp_severity = "low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: tmp_severity = "notification"; break;
	};
	ENGINE_LOG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
}


struct SDL_Window;

ModuleOpenGL::ModuleOpenGL() : context(nullptr)
{
}

// Destructor
ModuleOpenGL::~ModuleOpenGL()
{
}

// Called before render is available
bool ModuleOpenGL::Init()
{
	ENGINE_LOG("Creating Renderer context");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	context = SDL_GL_CreateContext(App->GetWindow()->window); // context creation

	GLenum err = glewInit();
	// … check for errors
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return false;
	}
	ENGINE_LOG("Using Glew %s", glewGetString(GLEW_VERSION)); // Should be 2.0
	
	ENGINE_LOG("Vendor: %s", glGetString(GL_VENDOR));
	ENGINE_LOG("Renderer: %s", glGetString(GL_RENDERER));
	ENGINE_LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	ENGINE_LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST); // Enable depth test
	glEnable(GL_CULL_FACE); // Enable cull backward faces
	glFrontFace(GL_CCW); // Front faces will be counter clockwise

	glDisable(GL_SCISSOR_TEST); // not used for now
	glDisable(GL_STENCIL_TEST); // not used for now

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT); // Enable output callbacks
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Output callbacks

	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true); // filter notifications
#endif 

	return true;
}

update_status ModuleOpenGL::PreUpdate()
{
	glViewport(0, 0, App->GetWindow()->screen_surface->w, App->GetWindow()->screen_surface->h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleOpenGL::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleOpenGL::PostUpdate()
{
	SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow()); // swap currentWindow
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleOpenGL::CleanUp()
{
	ENGINE_LOG("Destroying renderer");

	//Destroy window
	if (context != NULL)
	{
		SDL_GL_DeleteContext(context);
	}
	return true;
}

void ModuleOpenGL::WindowResized(unsigned width, unsigned height, unsigned windowID)
{
	SDL_Window* window = SDL_GetWindowFromID(windowID);
	if (window != nullptr)
	{
		SDL_Surface* screen_surface = SDL_GetWindowSurface(window);
		if (windowID == SDL_GetWindowID(App->GetWindow()->window)) // main window
		{
			App->GetCamera()->setAspectRatio(App->GetCamera()->getAspectRatio());
			glViewport(0, 0, width, height);
			ENGINE_LOG("Window Resized to %dx%d", width, height);
		}
			
	}
	else 
	{
		ENGINE_LOG("Error with Window Resize");
	}
}