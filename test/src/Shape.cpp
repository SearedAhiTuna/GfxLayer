
#include "Shape.h"

Shape::Buffer::Buffer(const size_t& _nverts, const size_t& _ndims):
	nverts(_nverts),
	ndims(_ndims)
{
	// Generate a VBO
	glGenBuffers(1, &vbo);

	// Generate the data
	data = new GLfloat[nverts * ndims];
	size = nverts * ndims * sizeof(GLfloat);

	// Copy the buffer data
	glStartBufferTX(vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glEndBufferTX();
}

Shape::Buffer::~Buffer()
{
	// Delete the VBO
	glDeleteBuffers(1, &vbo);

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
	buffers.emplace_back(nverts, ndims);
	return buffers.back().data;
}

void Shape::deleteBuffer(GLfloat* buf)
{
	for (auto it = buffers.begin(); it != buffers.end(); ++it)
	{
		if (it->data == buf)
		{
			buffers.erase(it);
			break;
		}
	}
}

void Shape::update()
{
	for (Buffer& buf : buffers)
	{
		// Copy the buffer data
		glStartBufferTX(buf.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, buf.size, buf.data);
		glEndBufferTX();
	}
}

void Shape::draw()
{
	if (!glOngoingVertexArrayTX())
		throw GraphicsError("Attetmpted to draw outside of vertex array transaction");

	// For each buffer
	size_t index = 0;
	for (Buffer& buf : buffers)
	{
		// Enable this attribute array
		glEnableVertexAttribArray(index);

		// Bind the buffer
		glStartBufferTX(buf.vbo);

		// Set the attribute pointer
		glVertexAttribPointer(
			index,              // attribute.
			buf.ndims,			// size
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
	glDrawArrays(mode, 0, buffers.front().nverts);

	// Disable the attribute arrays
	for (size_t i = 0; i < index; ++i)
	{
		glDisableVertexAttribArray(i);
	}
}