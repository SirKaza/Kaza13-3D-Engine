#pragma once
#include "Module.h"

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram() override;

	char* LoadShaderSource(const char* shader_file_name);
	unsigned CompileShader(unsigned type, const char* source);
	unsigned CreateProgram(unsigned vtx_shader, unsigned frg_shader);
};

