#version 330 core

//in vec2 uvs;
in vec4 particleColor;

out vec4 color;

//uniform sampler2D sprite;

void main()
{
    color = particleColor;
}  