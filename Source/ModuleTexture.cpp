#include "Globals.h"
#include "ModuleTexture.h"
#include "DirectXTex.h"
#include <GL/glew.h>
#include <filesystem>

ModuleTexture::ModuleTexture() : textureID(0)
{}

ModuleTexture::~ModuleTexture()
{}

unsigned int ModuleTexture::load(const char* imagePath)
{
	// const wchar_t* imagePath = L"Baboon.png"; // wide-character
	if (!loadTextureToCPU(imagePath))
		return 0;

	const DirectX::TexMetadata metadata = scratchImage->GetMetadata();

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, scratchImage->GetMetadata().mipLevels - 1);

	GLfloat borderColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f }; // yellow colour
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	for (size_t i = 0; i < metadata.mipLevels; ++i)
	{
		const DirectX::Image* mip = scratchImage->GetImage(i, 0, 0);
		glTexImage2D(GL_TEXTURE_2D, i, internalFormat, mip->width, mip->height, 0, format, type, mip->pixels);
	}

	if (metadata.mipLevels == 1)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	ENGINE_LOG("Texture loaded successfully (ID: %u)", textureID);	
	return textureID;
}

bool ModuleTexture::loadTextureToCPU(const char* imagePath)
{
	std::filesystem::path wImagePath = imagePath; // into std::filesystem::path
	scratchImage = std::make_unique<DirectX::ScratchImage>();
	HRESULT hr = DirectX::LoadFromDDSFile(wImagePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, *scratchImage);
	if (SUCCEEDED(hr))
	{
		return true;
	}

	hr = DirectX::LoadFromTGAFile(wImagePath.c_str(), nullptr, *scratchImage);
	if (SUCCEEDED(hr))
	{
		return true;
	}

	hr = DirectX::LoadFromWICFile(wImagePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, *scratchImage);
	if (SUCCEEDED(hr))
	{
		return true;
	}
	ENGINE_LOG("Error with texture %ls", imagePath);
	return false;
}

const char* ModuleTexture::DXGIFormatToString(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB";
	case DXGI_FORMAT_R8G8B8A8_UNORM: return "DXGI_FORMAT_R8G8B8A8_UNORM";
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB";
	case DXGI_FORMAT_B8G8R8A8_UNORM: return "DXGI_FORMAT_B8G8R8A8_UNORM";
	case DXGI_FORMAT_B5G6R5_UNORM: return "DXGI_FORMAT_B5G6R5_UNORM";
	default: return "Unknown DXGI_FORMAT";
	}
}

const int ModuleTexture::getWrapMode() const
{
	switch (wrapMode)
	{
		case GL_CLAMP_TO_BORDER: return 0;
		case GL_CLAMP: return 1;
		case GL_REPEAT: return 2;
		case GL_MIRRORED_REPEAT: return 3;
		default: return 2;
	}
}

void ModuleTexture::setWrapMode(GLenum wrapMode)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	glBindTexture(GL_TEXTURE_2D, 0);
}

const int ModuleTexture::getMinFilter() const
{
	if (!useMipmaps)
	{
		switch (minFilter)
		{
			case GL_NEAREST: return 0;
			case GL_LINEAR: return 1;
			default: return 0;
		}
	}
	else
	{
		switch (minFilter)
		{
			case GL_NEAREST_MIPMAP_NEAREST: return 0;
			case GL_LINEAR_MIPMAP_NEAREST: return 1;
			case GL_NEAREST_MIPMAP_LINEAR: return 2;
			case GL_LINEAR_MIPMAP_LINEAR: return 3;
			default: return 0;
		}
	}
}

void ModuleTexture::setMinFilter(GLenum filter)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

const int ModuleTexture::getMagFilter() const
{
	switch (magFilter)
	{
		case GL_NEAREST: return 0;
		case GL_LINEAR: return 1;
		default: return 0;
	}
}

void ModuleTexture::setMagFilter(GLenum filter)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleTexture::setUseMipMaps()
{
	glBindTexture(GL_TEXTURE_2D, textureID);

	glGenerateMipmap(GL_TEXTURE_2D);
	glGenerateTextureMipmap(textureID);

	glBindTexture(GL_TEXTURE_2D, 0);
}