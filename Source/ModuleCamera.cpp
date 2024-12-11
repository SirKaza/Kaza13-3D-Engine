#include "Globals.h"
#include "ModuleCamera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "Math/MathNamespace.h"
#include "Math/MathConstants.h"
#include "Math/float3.h"
#include "Math/float4.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"
#include "Math/Quat.h"
#include "imgui.h"

ModuleCamera::ModuleCamera()
{

}

ModuleCamera::~ModuleCamera()
{

}

bool ModuleCamera::Init()
{
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3(0.0f, 2.0f, 8.0f);
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
	// focus in main window
	if (SDL_GetKeyboardFocus() != App->GetWindow()->window || SDL_GetMouseFocus() != App->GetWindow()->window)
		return UPDATE_CONTINUE;

	const ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) // If ImGui has the mouse, dont make changes
		return UPDATE_CONTINUE;

	const ModuleInput* input = App->GetInput();
    const Uint8* keyboard = input->getKeyboard();
	float speed = CAMERA_SPEED;
    if (keyboard[SDL_SCANCODE_LSHIFT])
        speed *= 3;

    float deltaTime = App->GetDeltaTime();
	int dx, dy;
	Uint32 mouseButtons = input->getMouseButtons();
	int mouseWheel = input->getMouseWheelY();
	input->getMouseMotion(dx, dy);

	// Right click pressed
	if (mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		// Right click + WASD/QE: movement
		if (keyboard[SDL_SCANCODE_Q] || keyboard[SDL_SCANCODE_E] || keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_D])
		{
			float3 moveDirection = float3::zero;

			if (keyboard[SDL_SCANCODE_Q]) moveDirection += frustum.up;
			if (keyboard[SDL_SCANCODE_E]) moveDirection -= frustum.up;
			if (keyboard[SDL_SCANCODE_W]) moveDirection += frustum.front;
			if (keyboard[SDL_SCANCODE_S]) moveDirection -= frustum.front;
			if (keyboard[SDL_SCANCODE_A]) moveDirection -= frustum.WorldRight();
			if (keyboard[SDL_SCANCODE_D]) moveDirection += frustum.WorldRight();

			if (!moveDirection.IsZero())
			{
				moveDirection.Normalize();
				setPosition(frustum.pos + moveDirection * speed * deltaTime);
			}
		}
		
		// Alt + Right click: zoom
		else if (keyboard[SDL_SCANCODE_LALT]) 
		{
			float zoomSpeed = ZOOM_SPEED;
			float3 zoomDirection = frustum.front * (dy * zoomSpeed);

			setPosition(frustum.pos + zoomDirection);
		}
		
		// Right click: rotate
		else 
		{
			float sensitivity = ROTATE_SENSITIVITY;
			float yaw = DegToRad(dx * sensitivity);
			float pitch = DegToRad(dy * sensitivity);

			static float accPitch = frustum.up.y; 
			const float topRef = frustum.up.y;

			if ((PITCH_LIMIT + topRef >= accPitch + pitch) && (accPitch + pitch >= -PITCH_LIMIT + topRef))
			{
				accPitch += pitch;
				Quat yawRotation = Quat::RotateY(yaw);
				Quat pitchRotation = Quat::RotateAxisAngle(frustum.WorldRight(), pitch);

				frustum.front = (yawRotation * pitchRotation).Transform(frustum.front).Normalized();
				frustum.up = (yawRotation * pitchRotation).Transform(frustum.up).Normalized();

				setOrientation(frustum.front, frustum.up);
			}
		}
	}
	// Middle click: drag
	else if (mouseButtons & SDL_BUTTON(SDL_BUTTON_MIDDLE))
	{
		float sensitivity = DRAG_SENSITIVITY;
		
		float3 moveDirection = frustum.WorldRight() * (-dx * sensitivity) +
			frustum.up * (dy * sensitivity);

		setPosition(frustum.pos + moveDirection);
	}

	// Wheel movement for zooming
	else if (mouseWheel != 0)
	{
		float zoomSpeed = WHEEL_SPEED;
		float3 zoomDirection = frustum.front * (mouseWheel * zoomSpeed);

		setPosition(frustum.pos + zoomDirection);
	}

	return UPDATE_CONTINUE;
}

float ModuleCamera::getAspectRatio() const
{
	return (float)App->GetWindow()->screen_surface->w / App->GetWindow()->screen_surface->h;
}

// set the horizontal FOV keeping the aspect ratio
void ModuleCamera::setFOV(int horizontal)
{
	frustum.horizontalFov = DegToRad(horizontal);

	frustum.verticalFov = 2.f * atanf(tanf(frustum.horizontalFov * 0.5f) / getAspectRatio());
}

int ModuleCamera::getFOV() const
{

	return static_cast<int>(RadToDeg(frustum.horizontalFov));
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
	float3 right;

	float dotProduct = forward.Dot(up);

	if (dotProduct != 1.0f && dotProduct != -1.0f) {
		right = forward.Cross(up).Normalized();
	}
	else {
		right = float3(1.0f, 0.0f, 0.0f);
	}
	
	float3 up_corrected = right.Cross(forward).Normalized();

	float4x4 view;
	view.SetRow(0, float4(right, -eye.Dot(right)));
	view.SetRow(1, float4(up_corrected, -eye.Dot(up_corrected)));
	view.SetRow(2, float4(-forward, eye.Dot(forward)));
	view.SetRow(3, float4(0.0f, 0.0f, 0.0f, 1.0f));

	return view;
}