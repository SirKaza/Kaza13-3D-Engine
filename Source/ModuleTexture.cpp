#include "Globals.h"
#include "ModuleTexture.h"
#include "DirectXTex.h"

ModuleTexture::ModuleTexture()
{}

ModuleTexture::~ModuleTexture()
{}

bool ModuleTexture::Init() 
{
	const wchar_t* imagePath = L"Baboon.png"; // wide-character

	HRESULT hr = DirectX::LoadFromDDSFile(imagePath, DirectX::DDS_FLAGS_NONE, nullptr, scratchImage);
	if (SUCCEEDED(hr))
	{
		ENGINE_LOG("Texture %ls loaded correctly from DDS", imagePath);
		return true;
	}

	hr = DirectX::LoadFromTGAFile(imagePath, nullptr, scratchImage);
	if (SUCCEEDED(hr))
	{
		ENGINE_LOG("Texture %ls loaded correctly from TGA", imagePath);
		return true;
	}

	hr = DirectX::LoadFromWICFile(imagePath, DirectX::WIC_FLAGS_NONE, nullptr, scratchImage);
	if (SUCCEEDED(hr))
	{
		ENGINE_LOG("Texture %ls loaded correctly from WIC", imagePath);
		return true;
	}
	ENGINE_LOG("Error with texture %ls", imagePath);
	return false;
}

update_status ModuleTexture::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleTexture::CleanUp()
{
	return true;
}