#include "ModuleRender.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "Globals.h"
#include "FrameData.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"
#include "Math/MathNamespace.h" // math::pi
#include "Math/MathConstants.h"
#include <chrono>
#include <GL/glew.h>

ModuleRender::ModuleRender() : vbo(0), program_id(0)
{}

ModuleRender::~ModuleRender()
{}

bool ModuleRender::Init()
{
	//float vtx_data[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	float vtx_data[] = {	
		-1.0f, -1.0f, 0.0f, 
		1.0f, -1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f 
	};
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // set vbo active
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtx_data), vtx_data, GL_STATIC_DRAW);

	auto* program_module = App->GetProgram();
	char* vtx_shader = program_module->LoadShaderSource("./default_vertex.glsl");
	char* frg_shader = program_module->LoadShaderSource("./default_fragment.glsl");

	unsigned vtx_id = program_module->CompileShader(GL_VERTEX_SHADER, vtx_shader);
	unsigned frg_id = program_module->CompileShader(GL_FRAGMENT_SHADER, frg_shader);

	program_id = program_module->CreateProgram(vtx_id, frg_id);

	free(vtx_shader);
	free(frg_shader);
	return true;

}

update_status ModuleRender::Update()
{
	Frustum frustum = App->GetCamera()->getFrustum();

	model = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f),
		float4x4::RotateZ(pi / 4.0f),
		float3(2.0f, 1.0f, 1.0f));
	
	//float3 target = model.TranslatePart();
	//float3 targetPos(0.0f, 0.0f, 0.0f);

	App->GetCamera()->LookAt(frustum.pos, frustum.front, frustum.up);
	float4x4 view = App->GetCamera()->getViewMatrixInverted();

	float4x4 proj = App->GetCamera()->getProjectionMatrix();

	// Pass MVP as uniform to Vertex shader
	glUseProgram(program_id);
	glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

	// Bind buffer and vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	// size = 3 float per vertex
	// stride = 0 is equivalent to stride = sizeof(float)*3
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 1 triangle to draw = 3 vertices
	glDrawArrays(GL_TRIANGLES, 0, 3);

	UpdateFrameData();

	return UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	glDeleteBuffers(1, &vbo);

	return true;
}