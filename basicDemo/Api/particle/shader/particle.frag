#version 330 core

in vec2 uvs;
in vec4 particleColor;

out vec4 color;

uniform sampler2D texture1;

void main()
{
    color = texture(texture1,uvs);

    if(color.a < 0.4f)
        discard;
}  