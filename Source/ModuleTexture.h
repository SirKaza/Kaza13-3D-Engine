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

	bool loadTextureToCPU();
	GLuint getTextureID() const { return textureID; }

	const DirectX::ScratchImage& getScratchImage() const { return *scratchImage; }

	const int getWrapMode() const;
	void setWrapMode(const int newWrapMode);

	const int getMinFilter() const;
	void setMinFilter(const int newMinFilter);

	const int getMagFilter() const;
	void setMagFilter(const int newMagFilter);

	const bool getUseMipMaps() const;
	void setUseMipMaps();

	const char* DXGIFormatToString(DXGI_FORMAT format);

private:
	std::unique_ptr<DirectX::ScratchImage> scratchImage;
	const wchar_t* imagePath = L"Baboon.png"; // wide-character

	GLuint textureID = 0;
	const GLenum wrapMode = GL_REPEAT;
	const GLenum minFilter = GL_NEAREST;
	const GLenum magFilter = GL_LINEAR;
	const bool useMipmaps = true;
};

