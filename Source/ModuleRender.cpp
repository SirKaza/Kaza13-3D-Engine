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

	model = std::make_unique<Model>();
	model->load("BakerHouse.gltf");

	return true;
}

update_status ModuleRender::Update()
{
	float4x4 viewMatrix = App->GetCamera()->getViewMatrix(); // free camera
	viewMatrix.Transpose();
	float4x4 proj = App->GetCamera()->getProjectionMatrix();

	// Pass MVP as uniform to Vertex shader
	glUseProgram(program_id);

	glUniformMatrix4fv(1, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

	model->render();

	updateFrameData();

	return UPDATE_CONTINUE;
}

void ModuleRender::setModel(const char* pathModel)
{
	model = std::make_unique<Model>();
	model->load(pathModel);
}