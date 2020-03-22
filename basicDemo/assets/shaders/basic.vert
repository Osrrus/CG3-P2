#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 textCoord;
layout (location = 2) in vec3 Normal;

uniform mat4 view;
uniform mat4 projection;
// Vertex data out data
out vec3 vColor;

void main()
{
	vColor =Position;
    gl_Position = projection * view * vec4(Position, 1.0f);
}