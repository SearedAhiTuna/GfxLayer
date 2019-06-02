
#pragma once

#include "Libs.h"
#include "Shape.h"

#include <mutex>
#include <unordered_set>

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	void addShape(Shape* shape);
	void removeShape(Shape* shape);

	void render();

private:
	GLuint vao;

	std::unordered_set<Shape*> shapes;
	std::mutex shapesMutex;
};