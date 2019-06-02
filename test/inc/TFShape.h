
#pragma once

#include "Libs.h"
#include "Shape.h"

#include <vector>

class TFShape : public Shape
{
public:
	TFShape(const std::vector<vec4>& _verts, const GLenum& mode = GL_TRIANGLES);
	TFShape(const std::vector<vec3>& _verts, const GLenum& mode = GL_TRIANGLES);
	TFShape(const std::vector<vec2>& _verts, const GLenum& mode = GL_TRIANGLES);

	void tf(const mat4& mat);

	void tl(const vec3& d);
	void tl(const vec2& d);

	void rot(const GLfloat& angle, const vec3& axis = vec3(0.0f, 0.0f, 1.0f));
protected:
	virtual void update();

private:
	std::vector<vec4> verts;
	GLfloat* vertBuf;
};