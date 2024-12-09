#pragma once
#include "Module.h"
#include "DirectXTex.h"
#include <GL/glew.h>

enum WrapMode {
	CLAMP_TO_BORDER,
	CLAMP,
	REPEAT,
	MIRRORED_REPEAT
};

enum MipMapMode {
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR
};

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();
	update_status Update();
	bool CleanUp();

	unsigned int load(const char* imagePath);
	bool loadTextureToCPU(const char* imagePath);

	GLuint getTextureID() const { return textureID; }

	const DirectX::ScratchImage& getScratchImage() const { return *scratchImage; }

	const int getWrapMode() const;
	void setWrapMode(GLenum wrapMode);

	const int getMinFilter() const;
	void setMinFilter(GLenum newMinFilter);

	const int getMagFilter() const;
	void setMagFilter(GLenum filter);

	const bool getUseMipMaps() const { return useMipmaps; }
	void setUseMipMaps();

	const char* DXGIFormatToString(DXGI_FORMAT format);

private:
	std::unique_ptr<DirectX::ScratchImage> scratchImage;

	GLuint textureID = 0;
	const GLenum wrapMode = GL_REPEAT;
	const GLenum magFilter = GL_LINEAR;

	const GLenum minFilter = GL_LINEAR_MIPMAP_NEAREST;
	const bool useMipmaps = true;
};

