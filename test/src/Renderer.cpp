
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
	std::lock_guard<std::mutex> lk(shapesMutex);
	shapes.emplace(shape);
}

void Renderer::removeShape(Shape* shape)
{
	std::lock_guard<std::mutex> lk(shapesMutex);
	shapes.erase(shape);
}

void Renderer::render()
{
	if (!glfwOngoingWindowTX())
		throw GraphicsError("Attetmpted to render outside of window transaction");

	// Bind the VAO
	glStartVertexArrayTX(vao);

	// Draw all shapes
	std::lock_guard<std::mutex> lk(shapesMutex);
	for (Shape* shape : shapes)
	{
		if (shape->usesProgram())
		{
			const GLuint& id = programs[shape->getProgramIndex()]->getID();
			glUseProgram(id);
		}
		else
		{
			glUseProgram(0);
		}

		shape->draw();
	}

	// End the transaction
	glEndVertexArrayTX();
}

void Renderer::addProgram(const std::string& vert, const std::string& frag)
{
	programs.push_back(std::make_unique<Program>(vert, frag));
}