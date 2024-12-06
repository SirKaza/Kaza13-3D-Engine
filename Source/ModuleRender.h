#pragma once
#include "Module.h"
#include "Math/float4x4.h"

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status Update();
	bool CleanUp();

	void lookAtTarget();
	float4x4 getViewTransposed() const { return view.Transposed(); }

	float4x4 getModel() const { return model; }
	float4x4 getView() const { return view; }

private:
	unsigned vbo;
	unsigned program_id;

	float4x4 model, view;
};

