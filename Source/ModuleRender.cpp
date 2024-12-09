#include "ModuleRender.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleTexture.h"
#include "Globals.h"
#include "FrameData.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"
#include "Math/MathNamespace.h" // math::pi
#include "Math/MathConstants.h"
#include <chrono>
#include <GL/glew.h>
#include "Model.h"

ModuleRender::ModuleRender() : program_id(0)
{}

ModuleRender::~ModuleRender()
{}

bool ModuleRender::Init()
{
	auto* program_module = App->GetProgram();
	char* vtx_shader = program_module->LoadShaderSource("./default_vertex.glsl");
	char* frg_shader = program_module->LoadShaderSource("./default_fragment.glsl");

	unsigned vtx_id = program_module->CompileShader(GL_VERTEX_SHADER, vtx_shader);
	unsigned frg_id = program_module->CompileShader(GL_FRAGMENT_SHADER, frg_shader);

	program_id = program_module->CreateProgram(vtx_id, frg_id);

	free(vtx_shader);
	free(frg_shader);

	//model.load("TriangleWithoutIndices.gltf");
	//model.load("Triangle.gltf");
	//model.load("Box.gltf");
	//model.load("BoxInterleaved.gltf");
	model.load("BoxTextured.gltf");

	return true;
}

update_status ModuleRender::Update()
{
	modelMatrix = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f),
		float4x4::RotateZ(0.0f),
		float3(1.0f, 1.0f, 1.0f));

	//lookAtTarget(); // locked camera
	viewMatrix = App->GetCamera()->getViewMatrix(); // free camera

	float4x4 viewMatrixT = getViewMatrixTransposed();

	float4x4 proj = App->GetCamera()->getProjectionMatrix();

	// Pass MVP as uniform to Vertex shader
	glUseProgram(program_id);
	glUniformMatrix4fv(0, 1, GL_TRUE, &modelMatrix[0][0]);
	glUniformMatrix4fv(1, 1, GL_FALSE, &viewMatrixT[0][0]);
	glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

	model.render();

	UpdateFrameData();

	return UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	return true;
}

void ModuleRender::lookAtTarget()
{
	float3 target = modelMatrix.TranslatePart();
	Frustum frustum = App->GetCamera()->getFrustum();

	viewMatrix = App->GetCamera()->LookAt(frustum.pos, target, frustum.up);
}