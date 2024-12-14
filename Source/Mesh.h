#pragma once
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE 
#include "tiny_gltf.h"
#include "Math/float4x4.h"
#include "Math/float3.h"

class ModuleTexture;

class Mesh
{
public:
	Mesh();
	~Mesh();

	void load(const tinygltf::Model& model, const tinygltf::Mesh& srcMesh, const tinygltf::Primitive& primitive, size_t nodeIndex);
	void render(const std::vector<ModuleTexture*>& textures);
	void loadEBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);
	void createVAO();
	void setMatrix(const float4x4& globalMatrix) { modelMatrix = globalMatrix; }

	const float3& getMinPoint() const { return minPoint; }
	const float3& getMaxPoint() const { return maxPoint; }

	void setScale(const float3& scaling) { scale = scaling; }

private:
	unsigned vao;
	unsigned vbo;
	unsigned ebo;
	size_t numIndices;
	unsigned materialIndex;
	size_t numOfValues = 5;
	size_t vertexSize = sizeof(float) * numOfValues;
	float4x4 modelMatrix;
	bool hasIndices;
	float3 minPoint;
	float3 maxPoint;
	float3 scale;
};

