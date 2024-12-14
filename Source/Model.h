#pragma once
#include "Mesh.h"
#include <vector>
#include "Math/float4x4.h"
#include "Geometry/AABB.h"

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
	void loadNodeRecursive(const tinygltf::Model& model, int nodeIndex, const float4x4& parentMatrix);

	void render();

	float4x4& getMatrixFromNode(const tinygltf::Node& node) const;
	void setTexture(const char* texturePath);
	void calculateAABB();
	const AABB& getAABB() const { return modelAABB; }

private:
	std::vector<Mesh*> meshes;
	std::vector<ModuleTexture*> textures;
	AABB modelAABB;
};

