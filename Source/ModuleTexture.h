#pragma once
#include "Module.h"
#include "DirectXTex.h"

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();
	update_status Update();
	bool CleanUp();

private:
	DirectX::ScratchImage scratchImage;
};

