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
#include "Geometry/AABB.h"
#include "Math/float3.h"

Model::Model() : meshes(), textures(), scaling(float3::one), nodeCont(0)
{
	
}

Model::~Model()
{
	cleanTextures();
	cleanMeshes();
}

void Model::cleanTextures()
{
	if (!textures.empty())
	{
		for (ModuleTexture* texture : textures)
		{
			delete texture;
		}
		ENGINE_LOG("%d Texture cleaned up successfully", textures.size());
		textures.clear();
	}
}

void Model::cleanMeshes()
{
	if (!meshes.empty())
	{
		for (Mesh* mesh : meshes)
		{
			delete mesh;
		}
		ENGINE_LOG("%d Meshes cleaned up successfully", meshes.size());
		meshes.clear();
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
	else // model found successfully
	{
		// load all materials(textures) for model
		loadMaterials(model);

		if (!model.nodes.empty())
		{
			const float4x4& parentMatrix = float4x4::identity;
			if (!model.scenes.empty() && !model.scenes[0].nodes.empty())
			{
				// starting node rootNode
				loadNodeRecursive(model, model.scenes[0].nodes[0], parentMatrix);
			}
			else
			{
				loadNodeRecursive(model, 0, parentMatrix);
			}
			// get max min points model
			calculateAABB();
		}
	}
}

void Model::loadNodeRecursive(const tinygltf::Model& model, int nodeIndex, const float4x4& parentMatrix)
{
	// same nodes as meshes and there is scene
	if (nodeIndex < 0 || nodeIndex >= model.nodes.size())
		return;

	const tinygltf::Node& node = model.nodes[nodeIndex];

	float4x4& localMatrix = getMatrixFromNode(node);
	float4x4 globalMatrix = parentMatrix * localMatrix;

	// if node has mesh
	if (node.mesh >= 0 && node.mesh < model.meshes.size())
	{
		const tinygltf::Mesh& srcMesh = model.meshes[node.mesh];

		for (const tinygltf::Primitive& primitive : srcMesh.primitives)
		{
			Mesh* mesh = new Mesh();
			mesh->setMatrix(globalMatrix);
			mesh->setScale(scaling);
			mesh->load(model, srcMesh, primitive, nodeIndex);
			mesh->setName(model.meshes[node.mesh].name.c_str());
			ENGINE_LOG("%s mesh created", mesh->getName());
			meshes.push_back(mesh);
		}
	}

	// recursive nodes childrens
	for (size_t i = 0; i < node.children.size(); ++i)
	{
		int childIndex = node.children[i];
		loadNodeRecursive(model, childIndex, globalMatrix);
	}
}

void Model::render()
{
	for (Mesh* mesh : meshes)
	{
		mesh->render(textures);
	}
}

void Model::loadMaterials(const tinygltf::Model& srcModel)
{
	for (const auto& srcMaterial : srcModel.materials)
	{
		if (srcMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0) // texture
		{
			const tinygltf::Texture& texture = srcModel.textures[srcMaterial.pbrMetallicRoughness.baseColorTexture.index];
			const tinygltf::Image& image = srcModel.images[texture.source];
			loadTexture(image.uri.c_str());
		}
		else if (!srcMaterial.pbrMetallicRoughness.baseColorFactor.empty()) // colour
		{
			float4 color(
				static_cast<float>(srcMaterial.pbrMetallicRoughness.baseColorFactor[0]),
				static_cast<float>(srcMaterial.pbrMetallicRoughness.baseColorFactor[1]),
				static_cast<float>(srcMaterial.pbrMetallicRoughness.baseColorFactor[2]),
				(srcMaterial.pbrMetallicRoughness.baseColorFactor.size() > 3) ?
				static_cast<float>(srcMaterial.pbrMetallicRoughness.baseColorFactor[3]) : 1.0f);
			ModuleTexture* textureModule = new ModuleTexture();
			textureModule->setBaseColor(color);
			textureModule->setName(srcMaterial.name.c_str());
			textures.push_back(textureModule);
		}
	}
}

void Model::loadTexture(const char* texturePath)
{
	ModuleTexture* textureModule = new ModuleTexture();
	unsigned int textureId = textureModule->load(texturePath);
	if (textureId != 0) textures.push_back(textureModule);
}

float4x4& Model::getMatrixFromNode(const tinygltf::Node& node)
{
	float4x4 modelMatrix;
	if (!node.matrix.empty())
	{
		modelMatrix = float4x4(
			node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
			node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
			node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
			node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]
		);
		
		if (nodeCont == 0)
		{
			scaling = modelMatrix.GetScale();
		}
	}
	else
	{
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
			scaling = scale;
		}

		modelMatrix = float4x4::FromTRS(translation, rotationMatrix, scale);

		
	}
	nodeCont++;
	return modelMatrix;
}

void Model::setTexture(const char* texturePath)
{
	cleanTextures();
	loadTexture(texturePath);
}

void Model::calculateAABB()
{
	float3 globalMin(FLT_MAX, FLT_MAX, FLT_MAX);
	float3 globalMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (const Mesh* mesh : meshes)
	{
		const float3& meshMin = mesh->getMinPoint();
		const float3& meshMax = mesh->getMaxPoint();

		globalMin = math::Min(globalMin, meshMin);
		globalMax = math::Max(globalMax, meshMax);
	}

	modelAABB = AABB(globalMin, globalMax);
}