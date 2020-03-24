#version 430 core
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture

uniform sampler2D ssaoInput;

out vec4 color;
void main()
{
   // vec3 FragPos = texture(gPosition, vTexPos).rgb;
   
   vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, vTexPos + offset).r;
        }
    }
    color =  vec4(result /(4.0*4.0));
}