
#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec4 vertexNorm;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

// Output to frag shader
out vec4 norm;

void main()
{
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPos, 1);

	norm = MVP * vertexNorm;
}
