
#pragma once

#include "Libs.h"
#include "Program.h"

#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>

class Camera;
class Shape;

class Renderer
{
public:
    Renderer();
    Renderer(const Renderer& other) = delete;
    virtual ~Renderer();

    Renderer& operator=(const Renderer& rhs) = delete;

    void addShape(Shape& shape);
    void removeShape(Shape& shape);

    void render();

    void addProgram(const std::string& vert, const std::string& frag);

    Camera& getCamera() { return *camera; }

private:
    GLuint useProgram(const int& index);

private:
    GLuint vao;

    std::unordered_set<Shape*> shapes;
    std::mutex shapesMutex;

    std::vector<std::unique_ptr<Program>> programs;
    int curProgram{ -1 };

    std::unique_ptr<Camera> camera;
};