#version 430 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex Normals 
in vec3 vNor;
// Fregment position
in vec3 fragPos;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D text;

// Fragment Color
uniform vec3 viewPos;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gDiffuse;
layout (location = 2) out vec3 gNormal;
layout (location = 3) out vec3 gTexCoord;


void main()
{
    gPosition = fragPos.xyz;
    gDiffuse = texture(text, vTexPos).xyz;
    gNormal = normalize(vNor);
    gTexCoord = vec3(vTexPos, 0.0);
    
}