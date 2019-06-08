
#include "Shape.h"

#include <iostream>
#include <list>

static std::list<GLuint> used{};

Shape::Buffer::Buffer(const size_t& _nverts, const size_t& _ndims):
	nverts(_nverts),
	ndims(_ndims)
{
	GLuint temp;

	// Generate a VBO
	glGenBuffers(1, &vbo);

	// For some reason it sometimes gives the same one twice
	// and the weird thing is it doesn't give the same one twice
	// if I use a local variable, but if I use the struct variable it does
	// I seriously don't understand
	for (const GLuint& id : used)
	{
		if (vbo == id)
		{
			glGenBuffers(1, &temp);
			vbo = temp;
		}
	}

	used.push_back(vbo);

	// Generate the data
	data = new GLfloat[nverts * ndims];
	size = nverts * ndims * sizeof(GLfloat);
}

Shape::Buffer::~Buffer()
{
	// Delete the VBO
	glDeleteBuffers(1, &vbo);

	// Pull it out of the used list
	// that I have to maintain for some reason
	for (auto it = used.begin(); it != used.end(); ++it)
	{
		if (*it == vbo)
		{
			used.erase(it);
			break;
		}
	}

	// Delete the data
	delete[] data;
}

Shape::Shape(const GLenum& _mode):
	mode(_mode)
{
}

Shape::~Shape()
{
}

GLfloat* Shape::genBuffer(const size_t& nverts, const size_t& ndims)
{
	std::lock_guard<std::mutex> lk(buffersMutex);
	buffers.emplace_back(nverts, ndims);
	needsUpdate = true;
	return buffers.back().data;
}

void Shape::deleteBuffer(GLfloat* buf)
{
	std::lock_guard<std::mutex> lk(buffersMutex);
	for (auto it = buffers.begin(); it != buffers.end(); ++it)
	{
		if (it->data == buf)
		{
			buffers.erase(it);
			break;
		}
	}
	needsUpdate = true;
}

void Shape::update()
{
	std::lock_guard<std::mutex> lk(buffersMutex);
	needsUpdate = true;
}

void Shape::draw()
{
	std::lock_guard<std::mutex> lk(buffersMutex);

	if (buffers.empty())
	{
		return;
	}

	// Update buffers
	if (needsUpdate)
	{
		for (Buffer& buf : buffers)
		{
			// Copy the buffer data
			glStartBufferTX(buf.vbo);
			if (buf.init)
			{
				glBufferSubData(GL_ARRAY_BUFFER, 0, buf.size, buf.data);
			}
			else
			{
				glBufferData(GL_ARRAY_BUFFER, buf.size, buf.data, GL_STATIC_DRAW);
				buf.init = true;
			}
			glEndBufferTX();
		}

		needsUpdate = false;
	}

	if (!glOngoingVertexArrayTX())
		throw GraphicsError("Attetmpted to draw outside of vertex array transaction");

	// For each buffer
	GLuint index = 0;
	for (Buffer& buf : buffers)
	{
		// Enable this attribute array
		glEnableVertexAttribArray(index);

		// Bind the buffer
		glStartBufferTX(buf.vbo);

		// Set the attribute pointer
		glVertexAttribPointer(
			index,              // attribute.
			(GLint)buf.ndims,	// size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// End the transaction
		glEndBufferTX();

		// Increment index
		++index;
	}

	// Draw the arrays
	glDrawArrays(mode, 0, (GLsizei)buffers.front().nverts);

	// Disable the attribute arrays
	for (GLuint i = 0; i < index; ++i)
	{
		glDisableVertexAttribArray(i);
	}
}