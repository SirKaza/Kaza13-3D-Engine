#pragma once
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE 
#include "tiny_gltf.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void load(const tinygltf::Model& model, const tinygltf::Mesh& srcMesh, const tinygltf::Primitive& primitive);
	void render(const std::vector<unsigned>& textures);
	void loadEBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);
	void createVAO();

private:
	unsigned vao;
	unsigned vbo;
	unsigned ebo;
	unsigned numIndices;
	unsigned materialIndex;
	size_t numOfValues = 5;
	size_t vertexSize = sizeof(float) * numOfValues;
};
