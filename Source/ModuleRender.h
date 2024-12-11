#pragma once
#include "Module.h"
#include "Math/float4x4.h"
#include "Model.h"
#include <GL/glew.h>
#include <memory>

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender() override;

	bool Init() override;
	update_status Update() override;

	void lookAtTarget();

	float4x4 getViewMatrix() const { return viewMatrix; }
	float4x4 getViewMatrixTransposed() const { return viewMatrix.Transposed(); }
	float4x4 getModelMatrix() const { return modelMatrix; }

	unsigned getProgramID() const { return program_id; }

	void setModel(const char* pathModel);
	Model* getModel() const { return model.get(); } // we can modify

private:
	unsigned program_id;

	float4x4 modelMatrix, viewMatrix;
	std::unique_ptr<Model> model;
};

