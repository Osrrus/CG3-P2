#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
layout (location = 2) in vec2 vertexTexPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// Vertex data out data
out vec3 vColor;
out vec2 vText;

void main()
{
	vColor =vec3(1.0f);
	vText = vertexTexPos;
    gl_Position = projection * view * model *vec4(vertexPosition, 1.0f);
}