#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;

uniform mat4 view;
uniform mat4 projection;
// Vertex data out data

void main()
{
    gl_Position = projection * view * vec4(vertexPosition, 1.0f);
}