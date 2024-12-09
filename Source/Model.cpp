#define TINYGLTF_IMPLEMENTATION /* Only in one of the includes */
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE 
#include "Globals.h"
#include "Model.h"
#include "Mesh.h"
#include "Application.h"
#include "ModuleTexture.h"

Model::Model()
{
	
}

Model::~Model()
{
	for (unsigned &textureID : textures)
	{
		if (textureID != 0)
		{
			glDeleteTextures(1, &textureID);
			textureID = 0;
		}
	}
	textures.clear();
	ENGINE_LOG("Texture cleaned up successfully");

	for (Mesh* mesh : meshes)
	{
		delete mesh;
	}
	meshes.clear();
	ENGINE_LOG("Meshes cleaned up successfully");

}

void Model::load(const char* assetFileName)
{
	tinygltf::TinyGLTF gltfContext;
	tinygltf::Model model;

	std::string error, warning;
	bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, assetFileName);
	if (!loadOk)
	{
		ENGINE_LOG("Error loading %s: %s", assetFileName, error.c_str());
	}
	else // model found successfully
	{
		// load all materials for model
		loadMaterials(model);

		// load all meshes (primitives)
		for (const tinygltf::Mesh& srcMesh : model.meshes)
		{
			for (const tinygltf::Primitive& primitive : srcMesh.primitives)
			{
				Mesh* mesh = new Mesh();
				mesh->load(model, srcMesh, primitive);
				meshes.push_back(mesh);
			}
		}
	}
}

void Model::render()
{
	for (Mesh* mesh : meshes)
	{
		mesh->render();
	}
}

void Model::loadMaterials(const tinygltf::Model& srcModel)
{
	for (const auto& srcMaterial : srcModel.materials)
	{
		unsigned int textureId = 0;
		if (srcMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0)
		{
			const tinygltf::Texture& texture = srcModel.textures[srcMaterial.pbrMetallicRoughness.baseColorTexture.index];
			const tinygltf::Image& image = srcModel.images[texture.source];
			textureId = (App->GetTexture()->load(image.uri.c_str()));
		}
		if (textureId != 0) textures.push_back(textureId);
	}
}