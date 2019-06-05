
#pragma once

#include "Libs.h"

class Program
{
public:
	Program(const std::string& vert, const std::string& frag);
	Program(const Program& other) = delete;
	virtual ~Program();

	Program& operator=(const Program& other) = delete;

	const GLuint& getID() { return id; }

private:
	GLuint id;
};