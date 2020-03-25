#version 430 core


uniform vec3 colorDiff;
// Fragment Color
out vec4 color;


void main()
{
   
    color = vec4(colorDiff, 1.0f);
}