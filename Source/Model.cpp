#define TINYGLTF_IMPLEMENTATION /* Only in one of the includes */
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE 
#include "Globals.h"
#include "Model.h"
#include "Mesh.h"

Model::Model()
{
	
}

Model::~Model()
{
	ENGINE_LOG("Deleting meshes");
	for (Mesh* mesh : meshes)
	{
		delete mesh;
	}
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
	else
	{
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