#include "Globals.h"
#include "ModuleTexture.h"
#include "DirectXTex.h"
#include <GL/glew.h>

ModuleTexture::ModuleTexture() : textureID(0)
{}

ModuleTexture::~ModuleTexture()
{}

bool ModuleTexture::Init() 
{
	if (!loadTextureToCPU()) 
		return false;

	const DirectX::TexMetadata metadata = scratchImage.GetMetadata();

	GLenum internalFormat = 0, format = 0, type = 0;

	switch (metadata.format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		internalFormat = GL_RGBA8;
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		internalFormat = GL_RGBA8;
		format = GL_BGRA;
		type = GL_UNSIGNED_BYTE;
		break;
	case DXGI_FORMAT_B5G6R5_UNORM:
		internalFormat = GL_RGB8;
		format = GL_BGR;
		type = GL_UNSIGNED_BYTE;
		break;
	default:
		assert(false && "Unsupported format");
	}

	// Generate and link texture object
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, scratchImage.GetMetadata().mipLevels - 1);

	for (size_t i = 0; i < metadata.mipLevels; ++i)
	{
		const DirectX::Image* mip = scratchImage.GetImage(i, 0, 0);
		glTexImage2D(GL_TEXTURE_2D, i, internalFormat, mip->width, mip->height, 0, format, type, mip->pixels);
	}

	if (metadata.mipLevels == 1)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	ENGINE_LOG("Texture loaded successfully (ID: %u)", textureID);

	return true;
}

update_status ModuleTexture::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleTexture::CleanUp()
{
	if (textureID != 0)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
		ENGINE_LOG("Texture cleaned up successfully");
	}
	return true;
}

bool ModuleTexture::loadTextureToCPU()
{
	HRESULT hr = DirectX::LoadFromDDSFile(imagePath, DirectX::DDS_FLAGS_NONE, nullptr, scratchImage);
	if (SUCCEEDED(hr))
	{
		return true;
	}

	hr = DirectX::LoadFromTGAFile(imagePath, nullptr, scratchImage);
	if (SUCCEEDED(hr))
	{
		return true;
	}

	hr = DirectX::LoadFromWICFile(imagePath, DirectX::WIC_FLAGS_NONE, nullptr, scratchImage);
	if (SUCCEEDED(hr))
	{
		return true;
	}
	ENGINE_LOG("Error with texture %ls", imagePath);
	return false;
}