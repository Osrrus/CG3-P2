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


// Fragment Color
out vec4 color;
uniform vec3 viewPos;

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

vec3 intensityPointLight(PointLight pointLight, vec3 normal, vec3 viewdir, vec3 diffuseColorK)
{
    float n = 15.0f;

    vec3 LightDir = normalize(pointLight.pos - fragPos);
    vec3 R = reflect(-LightDir, normal);
    vec3 halfwayDir = normalize(LightDir + viewdir);
    
    vec3 ambient  = /*ka */ pointLight.ambientColor;
    if(!pointLight.on)
        return ambient;

    vec3 diffuse  = diffuseColorK * pointLight.diffuseColor * max(0.0, dot(normal, LightDir));
    vec3 specular =/* ks */ pointLight.specularColor * pow(max(0.0, dot(R, halfwayDir)), n);
    
    float dist = length(pointLight.pos - fragPos);
    float attenuation = 1.0f / (pointLight.attenuationK.x 
        + pointLight.attenuationK.y * dist
        + pointLight.attenuationK.z * (dist * dist));
    
    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;  
    
    return ambient + diffuse + specular;
    
}

void main()
{
    vec3 normal = normalize(vNor);
    vec3 ViewDir = normalize(viewPos - fragPos.xyz);

    vec3 result = vec3(0.0f); 
    result += intensiyLightDir(normal, ViewDir, texture2D(text, vTexPos).rgb);
//    vec3 result = intensiyLightDir(normal, ViewDir, vec3(1.0f,0.0f,1.0f));
    if(texture2D(text, vTexPos).a < 0.1)
        discard;
    for(int i = 0; i < 1; i++)
    {
        result += intensityPointLight(pointLights[i], normal, ViewDir, texture2D(text, vTexPos).rgb);
    }
    color = vec4(viewPos, 1.0f);
    color = vec4(result, 1.0f);
}