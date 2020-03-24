#version 430 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
// Attribute 2 of the vertex
layout (location = 1) in vec2 vertexTexPos;

// Vertex texture position
out vec2 vTexPos;

void main()
{
	vTexPos = vertexTexPos; 
    gl_Position =vec4((vertexPosition), 1.0f);
}