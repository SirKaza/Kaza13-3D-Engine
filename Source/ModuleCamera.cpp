#include "ModuleCamera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Math/MathNamespace.h"
#include "Math/MathConstants.h"
#include "Math/float4.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

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

void ModuleCamera::LookAt(const float3& eye, const float3& target, const float3& up)
{
	float3 forward = (target - eye).Normalized();
	float3 right;

	float dotProduct = forward.Dot(up);

	if (dotProduct != 1.0f && dotProduct != -1.0f) {
		right = forward.Cross(up).Normalized();
	}
	else {
		right = float3(1.0f, 0.0f, 0.0f);
	}
	
	float3 up_corrected = right.Cross(forward).Normalized();

	view.SetCol(0, float4(right, 0.0f));
	view.SetCol(1, float4(up_corrected, 0.0f));
	view.SetCol(2, float4(-forward, 0.0f));
	view.SetCol(3, float4(eye, 1.0f));
}