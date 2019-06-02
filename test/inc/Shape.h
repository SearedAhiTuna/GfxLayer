
#pragma once

#include "Libs.h"

#include <atomic>
#include <list>
#include <mutex>

class Shape
{
private:
	struct Buffer
	{
		GLuint vbo;

		size_t nverts;
		size_t ndims;

		GLfloat* data;
		size_t size;

		bool init{};

		Buffer(const size_t& _nverts, const size_t& _ndims);
		~Buffer();
	};

public:
	Shape(const GLenum& _mode = GL_TRIANGLES);
	virtual ~Shape();

	GLfloat* genBuffer(const size_t& nverts, const size_t& ndims);
	void deleteBuffer(GLfloat* buf);

	void update();
	void draw();

private:
	GLenum mode;

	std::list<Buffer> buffers;
	std::mutex buffersMutex;

	bool needsUpdate;
};