#pragma once
#include "Module.h"
#include "MathGeoLib.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool Init();
	update_status Update();
	bool CleanUp();

	Frustum getFrustum() const { return frustum; }
	float getAspectRatio() const;

	void setFOV(float horizontal);
	void setAspectRatio(float aspect);
	void setPlaneDistances(float nearPlane, float farPlane);
	void setPosition(const float3& position) { frustum.pos = position; }
	float3 getPosition() const { return frustum.pos; }
	void setOrientation(const float3& front, const float3& up) {
		frustum.front = front.Normalized();
		frustum.up = up.Normalized();
	}
	std::pair<float3, float3> getOrientation() const{ return {frustum.front, frustum.up}; }

	float4x4 LookAt(const float3& eye, const float3& target, const float3& up);

	float4x4 getProjectionMatrix() const;
	float4x4 getViewMatrix() const;

private:
	Frustum frustum;
};

