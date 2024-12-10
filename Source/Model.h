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

	void load(const char* assetFileName);
	void render();
	void loadMaterials(const tinygltf::Model& srcModel);

private:
	std::vector<Mesh*> meshes;
	std::vector<unsigned> textures;
};

