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

	unsigned getProgramID() const { return program_id; }
	void setModel(const char* pathModel);
	Model* getModel() const { return model.get(); } // we can modify object

private:
	unsigned program_id;
	std::unique_ptr<Model> model;
};

