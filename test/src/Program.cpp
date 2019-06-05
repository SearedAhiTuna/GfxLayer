
#include "Program.h"

#include "Shader.h"

Program::Program(const std::string& vert, const std::string& frag)
{
	Shader vertShader(GL_VERTEX_SHADER, vert);
	Shader fragShader(GL_FRAGMENT_SHADER, frag);

	id = glCreateProgram();

	glAttachShader(id, fragShader.getID());
	glAttachShader(id, vertShader.getID());

	glLinkProgram(id);

	glDetachShader(id, vertShader.getID());
	glDetachShader(id, fragShader.getID());
}

Program::~Program()
{
	glDeleteProgram(id);
}