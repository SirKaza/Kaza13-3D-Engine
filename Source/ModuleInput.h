#pragma once
#include "Module.h"
#include "Globals.h"

typedef unsigned __int8 Uint8;
typedef unsigned __int32 Uint32;

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput() override;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	const Uint8* getKeyboard() const { return keyboard; }
	const Uint32 getMouseButtons() const { return mouseButtons; }
	void getMousePosition(int& x, int& y) const { x = mouseX; y = mouseY; }
	void getMouseMotion(int& x, int& y) const { x = mouseMotionX; y = mouseMotionY; }
	int getMouseWheelY() const { return mouseWheelY; }

private:
	const Uint8 *keyboard = NULL;
	Uint32 mouseButtons = 0;
	int mouseX = 0, mouseY = 0;
	int mouseMotionX = 0, mouseMotionY = 0;
	int mouseWheelY = 0;
};