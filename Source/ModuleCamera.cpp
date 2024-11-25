#include "ModuleCamera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Math/MathNamespace.h"
#include "Math/MathConstants.h"
#include "Math/float4.h"

ModuleCamera::ModuleCamera()
{

}

ModuleCamera::~ModuleCamera()
{

}

bool ModuleCamera::Init()
{
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3(0.0f, 4.0f, 8.0f);
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;

	App->GetCamera()->setAspectRatio(getAspectRatio());

	return true;
}

update_status ModuleCamera::Update()
{

	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

float ModuleCamera::getAspectRatio() const
{
	return (float)App->GetWindow()->screen_surface->w / App->GetWindow()->screen_surface->h;
}

// set the horizontal FOV keeping the aspect ratio
void ModuleCamera::setFOV(float horizontal)
{
	frustum.horizontalFov = horizontal;

	frustum.verticalFov = 2.f * atanf(tanf(horizontal * 0.5f) / getAspectRatio());
}

// change the vertical FOV to meet the new aspect ratio
void ModuleCamera::setAspectRatio(float aspect)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect);
}

void ModuleCamera::setPlaneDistances(float nearPlane, float farPlane)
{
	frustum.nearPlaneDistance = nearPlane;
	frustum.farPlaneDistance = farPlane;
}

float4x4 ModuleCamera::LookAt(const float3& eye, const float3& target, const float3& up)
{
	float3 forward = (target - eye).Normalized();

	float3 right = forward.Cross(up).Normalized();  // Right direction

	float3 up_corrected = right.Cross(forward).Normalized();

	float3 position = eye;

	float4x4 cameraMatrix;
	cameraMatrix.SetCol(0, float4(right, 0.0f));
	cameraMatrix.SetCol(1, float4(up_corrected, 0.0f));
	cameraMatrix.SetCol(2, float4(forward, 0.0f));
	cameraMatrix.SetCol(3, float4(position, 1.0f));

	return cameraMatrix;
}

float4x4 ModuleCamera::getProjectionMatrix() const
{
	float4x4 projectionMatrix = frustum.ProjectionMatrix();

	// row major to colum major
	projectionMatrix = projectionMatrix.Transposed();

	return projectionMatrix;
}

float4x4 ModuleCamera::getViewMatrix() const
{
	float4x4 viewMatrix = frustum.ViewMatrix();

	// row major to colum major
	viewMatrix = viewMatrix.Transposed();

	return viewMatrix;
}
