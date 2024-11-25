#include "ModuleRender.h"
#include <GL/glew.h>
#include "ModuleProgram.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "FrameData.h"
#include <chrono>
#include "MathGeoLib.h"
#include "Globals.h"

float4x4 LookAt(const float3& eye, const float3& target, const float3& up);

ModuleRender::ModuleRender() : vbo(0), program_id(0)
{}

ModuleRender::~ModuleRender()
{}

bool ModuleRender::Init()
{
	//float vtx_data[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	float vtx_data[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	
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
	startTime = std::chrono::steady_clock::now();

	float aspect = (float)App->GetWindow()->screen_surface->w / App->GetWindow()->screen_surface->h;

	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3(0.0f, 0.0f, -8.0f);
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect);

	float4x4 model = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f),
		float4x4::RotateZ(pi / 4.0f),
		float3(2.0f, 1.0f, 1.0f));
	
	float3 target = model.TranslatePart();
	
	float3 targetPos(0.0f, 0.0f, 0.0f);
	view = LookAt(frustum.pos, targetPos, frustum.up); // cameraMatrix
	float4x4 rotationZ = float4x4::RotateX(-pi / 6.0f); // 30 degrees
	view = view * rotationZ; // Aplicar la rotación a la matriz de vista

	proj = frustum.ProjectionMatrix();

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

float4x4 LookAt(const float3& eye, const float3& target, const float3& up)
{
	float3 forward = (target - eye).Normalized();

	float3 right = up.Cross(forward).Normalized();  // Right direction

	float3 up_corrected = forward.Cross(right).Normalized();

	float3 position = eye;

	float4x4 cameraMatrix;
	cameraMatrix.SetCol(0, float4(right, 0.0f));
	cameraMatrix.SetCol(1, float4(up_corrected, 0.0f));
	cameraMatrix.SetCol(2, float4(-forward, 0.0f));
	cameraMatrix.SetCol(3, float4(position, 1.0f));

	return cameraMatrix;
}