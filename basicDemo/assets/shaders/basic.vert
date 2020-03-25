#version 430 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
// Atributte 1 of the vertex
layout (location = 1) in vec3 vertexNormal;
// Attribute 2 of the vertex
layout (location = 2) in vec2 vertexTexPos;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Vertex data out data

void main()
{
    gl_Position = projection * view * model * vec4((vertexPosition), 1.0f);
}