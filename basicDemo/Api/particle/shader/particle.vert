#version 330 core

layout (location = 0) in vec3 vertex;

out vec4 particleColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;

void main() {

    particleColor = color;
    gl_Position = projection * view * model *vec4(vertex, 1.0);
    
}