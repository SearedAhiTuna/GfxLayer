
#include "TFShape.h"

#include <glm/gtx/transform.hpp>

TFShape::TFShape(const std::vector<vec4>& _verts, const GLenum& mode):
	Shape(mode),
	verts(_verts)
{
	vertBuf = genBuffer(verts.size(), 4);
	update();
}

TFShape::TFShape(const std::vector<vec3>& _verts, const GLenum& mode) :
	Shape(mode)
{
	// Reserve space for all vertices
	verts.reserve(_verts.size());

	// Copy the vertices
	for (const vec3& v3 : _verts)
	{
		verts.emplace_back(v3.x, v3.y, v3.z, 1.0f);
	}

	vertBuf = genBuffer(verts.size(), 4);
	update();
}

TFShape::TFShape(const std::vector<vec2>& _verts, const GLenum& mode) :
	Shape(mode)
{
	// Reserve space for all vertices
	verts.reserve(_verts.size());

	// Copy the vertices
	for (const vec2& v2 : _verts)
	{
		verts.emplace_back(v2.x, v2.y, 0.0f, 1.0f);
	}

	vertBuf = genBuffer(verts.size(), 4);
	update();
}

void TFShape::tf(const mat4& mat)
{
	for (vec4& v : verts)
	{
		v = mat * v;
	}
	update();
}

void TFShape::tl(const vec3& d)
{
	tf(glm::translate(glm::mat4(), d));
}
void TFShape::tl(const vec2& d)
{

	tf(glm::translate(mat4(), vec3(d.x, d.y, 0.0f)));
}

void TFShape::rot(const GLfloat& angle, const vec3& axis)
{
	tf(glm::rotate(mat4(), angle, axis));
}

void TFShape::update()
{
	// Copy from the vertex array to the float array
	for (size_t v = 0; v < verts.size(); ++v)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			vertBuf[v * 4 + i] = verts[v][i];
		}
	}

	// Copy from the float array to the buffer
	Shape::update();
}