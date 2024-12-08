#pragma once
#include "Mesh.h"
#include <vector>

class Model
{
public:
	Model();
	~Model();

	void load(const char* assetFileName);
	void render();

private:
	std::vector<Mesh*> meshes;
};

