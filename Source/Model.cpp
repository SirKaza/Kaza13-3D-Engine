#define TINYGLTF_IMPLEMENTATION /* Only in one of the includes */
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE 
#include "Globals.h"
#include "Model.h"
#include "Mesh.h"
#include "Application.h"
#include "ModuleTexture.h"
#include "Math/float4x4.h"
#include "Math/float4.h"
#include "Math/Quat.h"

Model::Model() : meshes(), textures(), modelMatrix()
{
	
}

Model::~Model()
{
	cleanTextures();
	cleanMeshes();
}

void Model::cleanTextures()
{
	ENGINE_LOG("%d Texture cleaned up successfully", textures.size());
	for (ModuleTexture* texture : textures)
	{
		delete texture;
	}
	textures.clear();
}

void Model::cleanMeshes()
{
	ENGINE_LOG("%d Meshes cleaned up successfully", meshes.size());
	for (Mesh* mesh : meshes)
	{
		delete mesh;
	}
	meshes.clear();
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
		// load modelMatrix rootNode
		loadModelMatrix(model);

		// load all materials(textures) for model
		loadMaterials(model);

		int cont = 0;
		// load all meshes (primitives)
		for (const tinygltf::Mesh& srcMesh : model.meshes)
		{
			for (const tinygltf::Primitive& primitive : srcMesh.primitives)
			{
				Mesh* mesh = new Mesh();
				mesh->load(model, srcMesh, primitive, model.nodes[0].children[cont]);
				meshes.push_back(mesh);
			}
			cont++;
		}
	}
}

void Model::render()
{
	for (Mesh* mesh : meshes)
	{
		mesh->render(textures, modelMatrix);
	}
}

void Model::loadMaterials(const tinygltf::Model& srcModel)
{
	for (const auto& srcMaterial : srcModel.materials)
	{
		unsigned int textureId = 0;
		ModuleTexture* textureModule = nullptr;
		if (srcMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0)
		{
			const tinygltf::Texture& texture = srcModel.textures[srcMaterial.pbrMetallicRoughness.baseColorTexture.index];
			const tinygltf::Image& image = srcModel.images[texture.source];
			textureModule = new ModuleTexture();
			textureId = textureModule->load(image.uri.c_str());
		}
		if (textureId != 0) textures.push_back(textureModule);
	}
}

void Model::loadModelMatrix(const tinygltf::Model& model)
{
	const tinygltf::Node& node = model.nodes[0];

	// Translation
	float3 translation(0.0f, 0.0f, 0.0f);
	if (!node.translation.empty() && node.translation.size() == 3)
	{
		translation = float3(static_cast<float>(node.translation[0]), static_cast<float>(node.translation[1]), static_cast<float>(node.translation[2]));
	}

	// Rotation
	float4 rotationQuat(0.0f, 0.0f, 0.0f, 1.0f);
	if (!node.rotation.empty() && node.rotation.size() == 4)
	{
		rotationQuat = float4(static_cast<float>(node.rotation[0]), static_cast<float>(node.rotation[1]), static_cast<float>(node.rotation[2]), static_cast<float>(node.rotation[3]));
	}
	float4x4 rotationMatrix = Quat(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w).ToFloat4x4();

	// Scale
	float3 scale(1.0f, 1.0f, 1.0f);
	if (!node.scale.empty() && node.scale.size() == 3)
	{
		scale = float3(static_cast<float>(node.scale[0]), static_cast<float>(node.scale[1]), static_cast<float>(node.scale[2]));
	}

	modelMatrix = float4x4::FromTRS(translation, rotationMatrix, scale);
}

void Model::setTexture(const char* texturePath)
{
	cleanTextures();
	ModuleTexture* textureModule = new ModuleTexture();
	unsigned int textureId = textureModule->load(texturePath);
	if (textureId != 0) textures.push_back(textureModule);
}