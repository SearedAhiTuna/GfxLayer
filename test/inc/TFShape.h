
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

	void tfInt(const mat4& mat);

	void tlInt(const vec3& d);
	void tlInt(const vec2& d);

	void rotInt(const GLfloat& angle, const vec3& axis = vec3(0.0f, 0.0f, 1.0f));

	void tfExt(const bool& rel, const mat4& mat);

	void tlExt(const bool& rel, const vec3& d);
	void tlExt(const bool& rel, const vec2& d);

	void rotExt(const bool& rel, const GLfloat& angle, const vec3& axis = vec3(0.0f, 0.0f, 1.0f));
protected:
	virtual void update();

private:
	std::vector<vec4> verts;
	GLfloat* vertBuf;
};