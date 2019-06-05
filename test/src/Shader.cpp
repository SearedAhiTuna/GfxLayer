
#include "Shader.h"

#include <fstream>
#include <sstream>

Shader::Shader(const GLenum& shaderType, const std::string& fn)
{
	// Create the shader
	id = glCreateShader(shaderType);

	// Read the shader from the file
	std::string code;
	std::ifstream file(fn, std::ios::in);
	if (file.is_open())
	{
		std::stringstream ss;
		ss << file.rdbuf();
		code = ss.str();
		file.close();
	}
	else
	{
		throw GraphicsError("Failed to open shader file");
	}

	// Compile the shader
	char const* source = code.c_str();
	glShaderSource(id, 1, &source, NULL);
	glCompileShader(id);
}

Shader::~Shader()
{
	glDeleteShader(id);
}