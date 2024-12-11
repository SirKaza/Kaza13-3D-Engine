#include "ModuleProgram.h"
#include <GL/glew.h>
#include <stddef.h>

ModuleProgram::ModuleProgram()
{}

ModuleProgram::~ModuleProgram()
{}

char* ModuleProgram::LoadShaderSource(const char* shader_file_name)
{
	char* data = nullptr;
	FILE* file = nullptr;
	fopen_s(&file, shader_file_name, "rb");
	if (file)
	{
		// obtain file size:
		fseek(file, 0, SEEK_END);
		long lSize = ftell(file);
		rewind(file);
		

		// allocate memory to contain the whole file:
		data = reinterpret_cast<char*>(malloc(sizeof(char)* lSize + 1)); // +1 for '\0'
		if (data != NULL) {
			size_t result = fread(data, 1, lSize, file);
			data[lSize] = '\0';
			if (result == lSize)
				ENGINE_LOG("%s loaded into memory", shader_file_name);
		}
		fclose(file);
	}
	return data;
}

unsigned ModuleProgram::CompileShader(unsigned type, const char* source)
{
	unsigned shader_id = glCreateShader(type);
	glShaderSource(shader_id, 1, &source, 0);
	glCompileShader(shader_id);
	int res = GL_FALSE;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = reinterpret_cast<char*>(malloc(len));
			glGetShaderInfoLog(shader_id, len, &written, info);
			ENGINE_LOG("Log Info: %s", info);
			free(info);
		}
	}
	return shader_id;
}

unsigned ModuleProgram::CreateProgram(unsigned vtx_shader, unsigned frg_shader)
{
	unsigned program_id = glCreateProgram();
	glAttachShader(program_id, vtx_shader);
	glAttachShader(program_id, frg_shader);
	glLinkProgram(program_id);
	int res;
	glGetProgramiv(program_id, GL_LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = reinterpret_cast<char*>(malloc(len));
			glGetProgramInfoLog(program_id, len, &written, info);
			ENGINE_LOG("Program Log Info: %s", info);
			free(info);
		}
	}
	glDeleteShader(vtx_shader);
	glDeleteShader(frg_shader);
	return program_id;
}


