
#include "Renderer.h"

Renderer::Renderer()
{
	glGenVertexArrays(1, &vao);
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &vao);
}

void Renderer::addShape(Shape* shape)
{
	shapes.emplace(shape);
}

void Renderer::removeShape(Shape* shape)
{
	shapes.erase(shape);
}

void Renderer::render()
{
	if (!glfwOngoingWindowTX())
		throw GraphicsError("Attetmpted to render outside of window transaction");

	// Bind the VAO
	glStartVertexArrayTX(vao);

	// Draw all shapes
	for (Shape* shape : shapes)
	{
		shape->draw();
	}

	// End the transaction
	glEndVertexArrayTX();
}