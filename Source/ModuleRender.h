#pragma once
#include "Module.h"
#include "Math/float4x4.h"
#include "Model.h"

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status Update();
	bool CleanUp();

	void lookAtTarget();
	float4x4 getViewMatrixTransposed() const { return viewMatrix.Transposed(); }

	float4x4 getModelMatrix() const { return modelMatrix; }
	float4x4 getViewMatrix() const { return viewMatrix; }
	unsigned getProgramID() const { return program_id; }

private:
	unsigned vbo;
	unsigned program_id;

	float4x4 modelMatrix, viewMatrix;
	Model model;
};

