#pragma once
#include "Mesh.h"
#include <vector>
#include "Math/float4x4.h"
#include "Geometry/AABB.h"
#include "Math/float3.h"
#include "Math/float4.h"

class tinygltf::Model;

class Model
{
public:
	Model();
	~Model();

	void cleanTextures();
	void cleanMeshes();

	void load(const char* assetFileName);
	void loadMaterials(const tinygltf::Model& srcModel);
	void loadTexture(const char* texturePath);
	float4x4& loadModelMatrix(const tinygltf::Model& model, int rootNode) const;
	void loadNodeRecursive(const tinygltf::Model& model, int nodeIndex, const float4x4& parentMatrix, float3& scaling);

	void render();

	void getMatrixFromNode(const tinygltf::Node& node, float4x4& localMatrix, float3& scaling); // nodeCont modify here
	void setTexture(const char* texturePath);
	void calculateAABB();
	const AABB& getAABB() const { return modelAABB; }

	const std::vector<Mesh*>& getMeshes() const { return meshes; }
	const std::vector<ModuleTexture*>& getTextures() const { return textures; }

private:
	std::vector<Mesh*> meshes;
	std::vector<ModuleTexture*> textures; // materials (textures and colors)
	AABB modelAABB;
	int nodeCont;
};

