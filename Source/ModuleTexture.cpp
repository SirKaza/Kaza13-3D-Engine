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
	scratchImage = std::make_unique<DirectX::ScratchImage>();
	HRESULT hr = DirectX::LoadFromDDSFile(imagePath, DirectX::DDS_FLAGS_NONE, nullptr, *scratchImage);
	if (SUCCEEDED(hr))
	{
		return true;
	}

	hr = DirectX::LoadFromTGAFile(imagePath, nullptr, *scratchImage);
	if (SUCCEEDED(hr))
	{
		return true;
	}

	hr = DirectX::LoadFromWICFile(imagePath, DirectX::WIC_FLAGS_NONE, nullptr, *scratchImage);
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

void ModuleTexture::setWrapMode(const int newWrapMode)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	switch (newWrapMode)
	{
	case 0: // GL_CLAMP_TO_BORDER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		break;
	case 1: // GL_CLAMP
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case 2: // GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case 3: // GL_MIRRORED_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	default:
		ENGINE_LOG("Error changing Wrap Mode %d", newWrapMode);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

const int ModuleTexture::getMinFilter() const
{
	switch (minFilter)
	{
		case GL_NEAREST: return 0;
		case GL_LINEAR: return 1;
		default: return 0;
	}
}

void ModuleTexture::setMinFilter(const int newMinFilter)
{
	glBindTexture(GL_TEXTURE_2D, textureID);

	switch (newMinFilter)
	{
	case 0: // GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case 1: // GL_LINEAR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	default:
		ENGINE_LOG("Error changing Min Filter %d", newMinFilter);
		break;
	}

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

void ModuleTexture::setMagFilter(const int newMagFilter)
{
	glBindTexture(GL_TEXTURE_2D, textureID);

	switch (newMagFilter)
	{
	case 0: // GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case 1: // GL_LINEAR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	default:
		ENGINE_LOG("Error changing Min Filter %d", newMagFilter);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

const bool ModuleTexture::getUseMipMaps() const
{
	if (useMipmaps) return true;
	else return false;
}

void ModuleTexture::setUseMipMaps()
{
	glBindTexture(GL_TEXTURE_2D, textureID);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}