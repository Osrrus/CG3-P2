#version 430 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture

uniform sampler2D gPosition;
uniform sampler2D gDiffuse;
uniform sampler2D gNormal;
uniform sampler2D gTextCoord;

// Fragment Color
uniform vec3 viewPos;

out vec4 color;
void main()
{
    vec3 FragPos = texture(gPosition, vTexPos).rgb;
    vec3 Normal = texture(gNormal, vTexPos).rgb;
    vec3 TextCoord = texture(gTextCoord, vTexPos).rgb;
    vec3 Diffuse = texture(gDiffuse, vTexPos).rgb;
//    float Specular = texture(gDiffuse, vTexPos).a;
    
    color =  vec4(1.0f);
    color =  vec4(Normal,1.0f);
    color =  vec4(FragPos,1.0f);
    color =  vec4(Diffuse,1.0f);
}