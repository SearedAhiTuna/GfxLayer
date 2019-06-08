
#include "TFShape.h"

#include <glm/gtx/transform.hpp>

TFShape::TFShape(const std::vector<vec4>& _verts, const GLenum& mode):
	Shape(mode)
{
	bufferCreate<vec4>(_verts);
}

TFShape::TFShape(const std::vector<vec3>& _verts, const GLenum& mode) :
	Shape(mode)
{
	std::vector<vec4> tempVerts;

	// Reserve space for all vertices
	tempVerts.reserve(_verts.size());

	// Copy the vertices
	for (const vec3& v3 : _verts)
	{
		tempVerts.emplace_back(v3, 1);
	}

	bufferCreate<vec4>(tempVerts);
}

TFShape::TFShape(const std::vector<vec2>& _verts, const GLenum& mode) :
	Shape(mode)
{
	std::vector<vec4> tempVerts;

	// Reserve space for all vertices
	tempVerts.reserve(_verts.size());

	// Copy the vertices
	for (const vec2& v2 : _verts)
	{
		tempVerts.emplace_back(v2, 0, 1);
	}

	bufferCreate<vec4>(tempVerts);
}

void TFShape::tfInt(const mat4& mat)
{
	Vert4Buffer& geoBuf = *(Vert4Buffer*)buffers.front().get();

	for (vec4& v : geoBuf.verts)
	{
		v = mat * v;
	}

	geoBuf.update();
	update();
}

void TFShape::tlInt(const vec3& d)
{
	tfInt(glm::translate(glm::mat4(), d));
}
void TFShape::tlInt(const vec2& d)
{
	tfInt(glm::translate(mat4(), vec3(d.x, d.y, 0.0f)));
}

void TFShape::rotInt(const GLfloat& angle, const vec3& axis)
{
	tfInt(glm::rotate(mat4(), angle, axis));
}

void TFShape::tfExt(const bool& rel, const mat4& mat)
{
	if (rel)
		tfMat = tfMat * mat;
	else
		tfMat = mat;
}

void TFShape::tlExt(const bool& rel, const vec3& d)
{
	tfExt(rel, glm::translate(glm::mat4(), d));
}
void TFShape::tlExt(const bool& rel, const vec2& d)
{
	tfExt(rel, glm::translate(mat4(), vec3(d.x, d.y, 0.0f)));
}

void TFShape::rotExt(const bool& rel, const GLfloat& angle, const vec3& axis)
{
	tfExt(rel, glm::rotate(mat4(), angle, axis));
}
