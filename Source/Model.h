#pragma once
#include "Mesh.h"
#include <vector>
#include "Math/float4x4.h"

class tinygltf::Model;

class Model
{
public:
	Model();
	~Model();

	void cleanTextures();
	void cleanMeshes();

	void load(const char* assetFileName);
	void render();
	void loadMaterials(const tinygltf::Model& srcModel);
	void loadModelMatrix(const tinygltf::Model& model);
	void setTexture(const char* texturePath);

private:
	std::vector<Mesh*> meshes;
	std::vector<ModuleTexture*> textures;
	float4x4 modelMatrix;
};

