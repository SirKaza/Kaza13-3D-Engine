#include "ModuleCamera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
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
    const Uint8* keyboard = App->GetInput()->getKeyboard();
    float speed = 3.0f;
    if (keyboard[SDL_SCANCODE_LSHIFT])
        speed *= 3;
    bool moved = false;
    float3 newPos;
    float deltaTime = App->GetDeltaTime();

    if ((keyboard[SDL_SCANCODE_Q] || keyboard[SDL_SCANCODE_E]) && !(keyboard[SDL_SCANCODE_Q] && keyboard[SDL_SCANCODE_E])) // Q up E down
    {
        float3 moveDirection = frustum.up.Normalized();  // normalize to evade irregular movements
        if (keyboard[SDL_SCANCODE_Q])
        {
            newPos = frustum.pos + moveDirection * speed * deltaTime;
        }
        else if (keyboard[SDL_SCANCODE_E])
        {
            newPos = frustum.pos - moveDirection * speed * deltaTime;
        }
        setPosition(newPos);
        moved = true;
    }

    if ((keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_S]) && !(keyboard[SDL_SCANCODE_W] && keyboard[SDL_SCANCODE_S])) // W forward S backwards
    {
        float3 moveDirection = frustum.front.Normalized();  // normalize to evade irregular movements

        if (keyboard[SDL_SCANCODE_W])
        {
            newPos = frustum.pos + moveDirection * speed * deltaTime;

        }
        else if (keyboard[SDL_SCANCODE_S])
        {
            newPos = frustum.pos - moveDirection * speed * deltaTime;
        }
        setPosition(newPos);
        moved = true;
    }

    if ((keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_D]) && !(keyboard[SDL_SCANCODE_A] && keyboard[SDL_SCANCODE_D])) // A left D right
    {
        float3 moveDirection = frustum.WorldRight().Normalized();  // normalize to evade irregular movements

        if (keyboard[SDL_SCANCODE_A])
        {
            newPos = frustum.pos - moveDirection * speed * deltaTime;
        }
        else if (keyboard[SDL_SCANCODE_D])
        {
            newPos = frustum.pos + moveDirection * speed * deltaTime;
        }
        setPosition(newPos);
        moved = true;
    }

    if (moved) {
        ENGINE_LOG("Camera Pos: (%.2f, %.2f, %.2f)/ Front: (%.2f, %.2f, %.2f)/ Up: (%.2f, %.2f, %.2f)", frustum.pos.x, frustum.pos.y, frustum.pos.z, 
            frustum.front.x, frustum.front.y, frustum.front.z, frustum.up.x, frustum.up.y, frustum.up.z);
    }

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
void ModuleCamera::setFOV(int horizontal)
{
	frustum.horizontalFov = horizontal * (math::pi / 180.0f); // degToRad

	frustum.verticalFov = 2.f * atanf(tanf(frustum.horizontalFov * 0.5f) / getAspectRatio());
}

int ModuleCamera::getFOV() const
{
    return static_cast<int>(frustum.horizontalFov * (180.0f / math::pi)); // radToDeg
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

	view.SetRow(0, float4(right, -eye.Dot(right)));
	view.SetRow(1, float4(up_corrected, -eye.Dot(up_corrected)));
	view.SetRow(2, float4(-forward, eye.Dot(forward)));
	view.SetRow(3, float4(0.0f, 0.0f, 0.0f, 1.0f));
}