#pragma once
#include "Module.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status Update();
	bool CleanUp();

	Frustum getFrustum() const { return frustum; }
	float4x4 getModel() const { return model; }
	float4x4 getView() const { return view; }
	float4x4 getProj() const { return proj; }

private:
	unsigned vbo;
	unsigned program_id;
	Frustum frustum;
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};

