#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE 
#include "tiny_gltf.h"
#include "Mesh.h"
#include <GL/glew.h>
#include "SDL/include/SDL.h"
#include "Math/float3.h"
#include "Math/float2.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTexture.h"
#include "Math/float4x4.h"
#include "Math/float4.h"
#include "Math/Quat.h"

Mesh::Mesh() : vao(0), vbo(0), ebo(0), numIndices(0), materialIndex(0)
{}

Mesh::~Mesh()
{
	glBindVertexArray(0);

	if (vao != 0)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
		
	if (ebo != 0)
	{
		glDeleteBuffers(1, &ebo);
		ebo = 0;
	}
}

void Mesh::load(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive, size_t nodeIndex)
{
	const auto& itPos = primitive.attributes.find("POSITION");
	const auto& itTex = primitive.attributes.find("TEXCOORD_0");

	if (itPos != primitive.attributes.end())
	{
		const tinygltf::Accessor& posAcc = model.accessors[itPos->second];
		SDL_assert(posAcc.type == TINYGLTF_TYPE_VEC3);
		SDL_assert(posAcc.componentType == GL_FLOAT);
		const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView];
		const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
		const unsigned char* bufferPos = &(posBuffer.data[posAcc.byteOffset + posView.byteOffset]);

		numIndices = posAcc.count;
		materialIndex = primitive.material;

		// uvs
		bool hasUVs = (itTex != primitive.attributes.end());
		const tinygltf::Accessor* uvAcc = nullptr;
		const tinygltf::BufferView* uvView = nullptr;
		const unsigned char* bufferUV = nullptr;

		if (hasUVs)
		{
			uvAcc = &model.accessors[itTex->second];
			SDL_assert(uvAcc->type == TINYGLTF_TYPE_VEC2);
			SDL_assert(uvAcc->componentType == GL_FLOAT);
			uvView = &model.bufferViews[uvAcc->bufferView];
			const tinygltf::Buffer& uvBuffer = model.buffers[uvView->buffer];
			bufferUV = &(uvBuffer.data[uvAcc->byteOffset + uvView->byteOffset]);
		}

		size_t totalSize = vertexSize * posAcc.count;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

		float* ptr = reinterpret_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

		size_t posStride = (posView.byteStride > 0) ? posView.byteStride : sizeof(float) * 3;
		size_t uvStride = (hasUVs && uvView->byteStride > 0) ? uvView->byteStride : sizeof(float) * 2;

		for (size_t i = 0; i < posAcc.count; ++i)
		{
			// Assign Position 
			float3 position = *reinterpret_cast<const float3*>(bufferPos);
			ptr[0] = position.x;
			ptr[1] = position.y;
			ptr[2] = position.z;
			bufferPos += posStride;

			// Assign TexCoords
			if (hasUVs)
			{
				float2 uv = *reinterpret_cast<const float2*>(bufferUV);
				ptr[3] = uv.x;
				ptr[4] = uv.y;
				bufferUV += uvStride;
			}
			else
			{
				// Default values
				ptr[3] = 0.0f;
				ptr[4] = 0.0f;
			}
			
			ptr += numOfValues;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	loadModelMatrix(model, mesh, nodeIndex);
	loadEBO(model, mesh, primitive);
	createVAO();
}

void Mesh::render(const std::vector<ModuleTexture*>& textures, const float4x4& modelMatrix)
{
	glUseProgram(App->GetRender()->getProgramID());
	float4x4 finalModelMatrix = modelMatrix * meshModelMatrix;
	glUniformMatrix4fv(0, 1, GL_TRUE, &finalModelMatrix[0][0]);

	if (materialIndex < textures.size()) // index not invalid
	{
		glActiveTexture(GL_TEXTURE0 + materialIndex);
		glBindTexture(GL_TEXTURE_2D, textures[materialIndex]->getTextureID());
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numIndices), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);	
}

void Mesh::loadEBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
	if (primitive.indices >= 0)
	{
		const tinygltf::Accessor& indAcc = model.accessors[primitive.indices];
		const tinygltf::BufferView& indView = model.bufferViews[indAcc.bufferView];
		const unsigned char* buffer = &(model.buffers[indView.buffer].data[indAcc.byteOffset +
			indView.byteOffset]);
		size_t num_indices = indAcc.count;
		numIndices = indAcc.count;

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indAcc.count, nullptr, GL_STATIC_DRAW);
		unsigned int* ptr = reinterpret_cast<unsigned int*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

		if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) // unsigned int
		{
			const uint32_t* bufferInd = reinterpret_cast<const uint32_t*>(buffer);
			for (uint32_t i = 0; i < num_indices; ++i) ptr[i] = bufferInd[i];
		}
		else if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) // unsigned short
		{
			const uint16_t* bufferInd = reinterpret_cast<const uint16_t*>(buffer);
			for (uint16_t i = 0; i < num_indices; ++i) ptr[i] = static_cast<unsigned int>(bufferInd[i]); // cast from short to int
		}
		else if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE) // unsigned byte
		{
			const uint8_t* bufferInd = reinterpret_cast<const uint8_t*>(buffer);
			for (uint8_t i = 0; i < num_indices; ++i) ptr[i] = static_cast<unsigned int>(bufferInd[i]); // cast from byte to int
		}
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	}
}

void Mesh::createVAO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexSize), reinterpret_cast<void*>(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(vertexSize), reinterpret_cast<void*>(sizeof(float) * 3));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBindVertexArray(0);
}

void Mesh::loadModelMatrix(const tinygltf::Model& model, const tinygltf::Mesh& mesh, size_t nodeIndex)
{
	const tinygltf::Node& node = model.nodes[nodeIndex];

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

	meshModelMatrix = float4x4::FromTRS(translation, rotationMatrix, scale);
}
