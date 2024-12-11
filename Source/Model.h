#pragma once
#include "Mesh.h"
#include <vector>
#include "Math/float4x4.h"
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
	void render();
	void loadMaterials(const tinygltf::Model& srcModel);
	void loadTexture(const char* texturePath);
	void loadModelMatrix(const tinygltf::Model& model);

	void setBaseColor(const float4 color) { baseColor = color; }
	float4 getBaseColor() const { return baseColor; }

	void setTexture(const char* texturePath);

private:
	std::vector<Mesh*> meshes;
	std::vector<ModuleTexture*> textures;
	float4x4 modelMatrix;
	float4 baseColor;
};

