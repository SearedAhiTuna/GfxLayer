
#pragma once

#include "Libs.h"

class Camera
{
public:
	void tl(const bool& relative, const vec3& d);
	void tl(const bool& relative, const vec2& d);

	void rot(const bool& relative, const GLfloat& angle, const vec3& axis = vec3(0.0f, 0.0f, 1.0f));

	vec4 getV();
	vec4 getP();
private:
	vec4 tlMat;
	vec4 rotMat;
};