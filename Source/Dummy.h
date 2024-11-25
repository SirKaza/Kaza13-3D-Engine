#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleDummy : public Module
{
	bool Init()
	{
		ENGINE_LOG("Dummy Init!");
		return true;
	}

	bool CleanUp()
	{
		ENGINE_LOG("Dummy CleanUp!");
		return true;
	}
};