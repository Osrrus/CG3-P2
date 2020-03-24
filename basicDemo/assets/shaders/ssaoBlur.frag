#version 430 core
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture

uniform sampler2D gPosition;
uniform sampler2D gDiffuse;
uniform sampler2D gNormal;
uniform sampler2D noiseText;

uniform vec3 samples[64];

// Fragment Color
uniform vec3 viewPos;
uniform float radius;
uniform float bias;
uniform float wWidth;
uniform float wHeight;

const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 
uniform mat4 projection;
int kernelSize = 64;

out vec4 color;
void main()
{
    vec3 FragPos = texture(gPosition, vTexPos).rgb;
    vec3 Normal = texture(gNormal, vTexPos).rgb;
    vec3 Diffuse = texture(gDiffuse, vTexPos).rgb;
    vec3 noise = normalize(texture(noiseText, vTexPos * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(noise - Normal * dot(noise, Normal));
    vec3 bitangent = cross(Normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, Normal);
    
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 _sample = TBN * samples[i]; // from tangent to view-space
        _sample = FragPos + _sample * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(_sample, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(FragPos.z - sampleDepth));
        occlusion += (sampleDepth >= _sample.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);

    color =  vec4(occlusion);
}