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
//Directional Light
struct DirLight {
    vec3 pos;
    vec3 dir;
    vec3 specular;
    vec3 diffuse;
    vec3 ambient;
    bool on;
};
uniform DirLight dirLight;

//Point light
struct PointLight {    
    vec3 pos;
    vec3 attenuationK; // k0 k1 k2
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    bool on;
};    
uniform PointLight pointLights[1];

vec3 intensiyLightDir(vec3 Normal, vec3 ViewDir, vec3 diffuseColorK)
{
    float n = 15.0f;
    //Directional light
    vec3 LightDir = normalize(-dirLight.dir); // Solo usamos la entrada del tweakbar
    vec3 reflectDir = reflect(-LightDir, Normal);
    vec3 halfwayDir = normalize(LightDir + ViewDir);
    //Material with lightDir components
    vec3 ambient  = /*ka */ dirLight.ambient;
    if(!dirLight.on)
        return ambient;
    vec3 diffuse  = diffuseColorK * /*dirLight.diffuse */ max(0.0f, dot(Normal, LightDir));
    vec3 specular = /*ks */ dirLight.specular * pow(max(0.0f, dot(reflectDir, halfwayDir)), n);

    return ambient + diffuse + specular;
}

vec3 intensityPointLight(PointLight pointLight, vec3 normal, vec3 viewdir, vec3 Diffuse)
{
    float n = 15.0f;
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 FragPos = texture(gPosition, vTexPos).rgb;

    vec3 LightDir = normalize(pointLight.pos - FragPos);
    vec3 R = reflect(-LightDir, normal);
    vec3 halfwayDir = normalize(LightDir + viewdir);
    
    vec3 ambient  = /*ka */ pointLight.ambientColor;
    if(!pointLight.on)
        return ambient;

    vec3 diffuse  = Diffuse * pointLight.diffuseColor * max(0.0, dot(normal, LightDir));
    vec3 specular =/* ks */ pointLight.specularColor * pow(max(0.0, dot(R, halfwayDir)), n);
    
    float dist = length(pointLight.pos - FragPos);
    float attenuation = 1.0f / (pointLight.attenuationK.x 
        + pointLight.attenuationK.y * dist
        + pointLight.attenuationK.z * (dist * dist));
    
    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;  
    
    return ambient + diffuse + specular;
    
}

out vec4 color;
void main()
{
    vec3 FragPos = texture(gPosition, vTexPos).rgb;
    vec3 Normal = texture(gNormal, vTexPos).rgb;
    vec3 TextCoord = texture(gTextCoord, vTexPos).rgb;
    vec3 Diffuse = texture(gDiffuse, vTexPos).rgb;


//    float Specular = texture(gDiffuse, vTexPos).a;
    vec3 viewDir  = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0f);
    result += intensityPointLight(pointLights[0], Normal, viewDir, Diffuse);
    result += intensiyLightDir(Normal, viewDir, Diffuse);
    color =  vec4(1.0f);
    color =  vec4(FragPos,1.0f);
    color =  vec4(Diffuse,1.0f);
    color =  vec4(result,1.0f);
}