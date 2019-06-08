
#include "Renderer.h"

#include "Camera.h"
#include "Shape.h"

#define MVP_MATRIX "MVP"

Renderer::Renderer()
{
	glGenVertexArrays(1, &vao);

	camera = std::make_unique<Camera>();
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &vao);
}

void Renderer::addShape(Shape& shape)
{
	std::lock_guard<std::mutex> lk(shapesMutex);
	shapes.emplace(&shape);
}

void Renderer::removeShape(Shape& shape)
{
	std::lock_guard<std::mutex> lk(shapesMutex);
	shapes.erase(&shape);
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
			GLuint progID = useProgram(shape->getProgramIndex());

			if (progID)
			{
				GLint mvpID = glGetUniformLocation(progID, MVP_MATRIX);

				if (mvpID != -1)
				{
					mat4 mvpMat{};

					mvpMat = shape->getTF() * mvpMat;

					mvpMat = camera->getV() * mvpMat;

					mvpMat = camera->getP() * mvpMat;

					glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvpMat[0][0]);
				}
			}
		}
		else
		{
			useProgram(-1);
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

GLuint Renderer::useProgram(const int& index)
{
	if (index == -1)
	{
		if (index != curProgram)
		{
			curProgram = -1;
			glUseProgram(0);
		}
		
		return 0;
	}
	else
	{
		if (index != curProgram)
		{
			curProgram = index;
			glUseProgram(programs[curProgram]->getID());
		}
		
		return programs[curProgram]->getID();
	}
}