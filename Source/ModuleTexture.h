#pragma once
#include "Module.h"
#include "DirectXTex.h"
#include <GL/glew.h>

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();
	update_status Update();
	bool CleanUp();

	GLuint getTextureID() const { return textureID; }
	bool loadTextureToCPU();

private:
	DirectX::ScratchImage scratchImage;
	const wchar_t* imagePath = L"Baboon.png"; // wide-character

	GLuint textureID = 0;
	const GLenum wrapMode = GL_REPEAT;
	const GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR;
	const GLenum magFilter = GL_LINEAR;
};

