#include "Globals.h"
#include "ModuleCamera.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "Model.h"
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

	focusModel();

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


	// Alt pressed
	if (keyboard[SDL_SCANCODE_LALT])
	{
		// Alt + Left click: orbit
		if ((mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT)))
		{
			float sensitivity = ORBITAL_SENSITIVITY;

			float yaw = DegToRad(dx * sensitivity);
			float pitch = DegToRad(dy * sensitivity);

			rotateCamera(yaw, pitch, sensitivity); // rotate first

			focusModel(); // then orbit around model
		}
		// Alt + Right click: zoom
		else if (mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT))
		{
			float zoomSpeed = ZOOM_SPEED;
			float3 zoomDirection = frustum.front * (dy * zoomSpeed);

			setPosition(frustum.pos + zoomDirection);
		}
	}
	// F pressed: focus
	else if (keyboard[SDL_SCANCODE_F])
	{
		focusModel();
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
	// Right click pressed
	else if (mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT))
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
		// Right click: rotate
		else 
		{
			float sensitivity = ROTATE_SENSITIVITY;
			float yaw = DegToRad(dx * sensitivity);
			float pitch = DegToRad(dy * sensitivity);

			rotateCamera(yaw, pitch, sensitivity);
		}
	}
	return UPDATE_CONTINUE;
}

void ModuleCamera::rotateCamera(float yaw, float pitch, float sensitivity)
{
	static float accumulatedPitch = frustum.up.z;

	float newPitch = accumulatedPitch + pitch;
	if (newPitch <= PITCH_LIMIT && newPitch >= -PITCH_LIMIT)
	{
		accumulatedPitch += pitch;

		// rotation with Quats
		Quat yawRotation = Quat::RotateY(yaw);
		Quat pitchRotation = Quat::RotateAxisAngle(frustum.WorldRight(), pitch);

		// rotate camera
		float3 front = (yawRotation * pitchRotation).Transform(frustum.front);
		float3 up = (yawRotation * pitchRotation).Transform(frustum.up);

		setOrientation(front, up);
	}
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

float4x4 ModuleCamera::lookAt(const float3& eye, const float3& target, const float3& up) const
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

	float4x4 viewMatrix;
	viewMatrix.SetRow(0, float4(right, -eye.Dot(right)));
	viewMatrix.SetRow(1, float4(up_corrected, -eye.Dot(up_corrected)));
	viewMatrix.SetRow(2, float4(-forward, eye.Dot(forward)));
	viewMatrix.SetRow(3, float4(0.0f, 0.0f, 0.0f, 1.0f));

	return viewMatrix;
}

void ModuleCamera::focusModel()
{
	AABB modelAABB = App->GetRender()->getModel()->getAABB();
	float3 center = modelAABB.CenterPoint();

	float distance = (modelAABB.maxPoint - modelAABB.minPoint).Length() * 1.5f;

	float3 direction = frustum.front.Normalized();
	float3 newPosition = center - direction * distance;

	setPosition(newPosition);

	float4x4 viewMatrix = lookAt(newPosition, center, frustum.up);

	setOrientation((center - newPosition), float3(viewMatrix[1][0], viewMatrix[1][1], viewMatrix[1][2]));
}