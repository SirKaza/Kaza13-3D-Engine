#pragma once
#include "Module.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

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

	void setFOV(int horizontal);
	int getFOV() const;
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

	float4x4 getProjectionMatrix() const { return frustum.ProjectionMatrix(); }
	float4x4 getViewMatrix() const { return frustum.ViewMatrix(); }

private:
	Frustum frustum;
};

template <typename T>
T Clamp(T value, T min, T max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}