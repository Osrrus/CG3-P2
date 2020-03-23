#version 330 core
// Vertex color (interpolated/fragment)
in vec3 Normal;
in vec3 fragPos;
in vec2 texCoord;

//Texture
uniform sampler2D text;
uniform sampler2D specMap;

//Camera
uniform vec3 viewPos;

//Directional Light
struct DirLight {
    vec3 lightDir;
    vec3 specular;
    vec3 diffuse;
    vec3 ambient;
    bool on;
};
uniform DirLight dirLight;
//Point Light
//struct PointLight {    
//    vec3 position;
//    vec3 attenuationK; // k0 k1 k2
//    vec3 ambientColor;
//    vec3 diffuseColor;
//    vec3 specularColor;
//    bool on;
//};    
//uniform PointLight pointLights[2];
//

//Model Material
//uniform vec3 ka;
//uniform vec3 kd;
//uniform vec3 ks;
//uniform float n;
uniform bool albedo;

// Fragment Color
out vec4 color;

vec3 intensiyLightDir(vec3 Normal, vec3 ViewDir, vec3 diffuseColorK)
{
    float n = 55.0f;
    //Directional light
    vec3 LightDir = normalize(-dirLight.lightDir); // Solo usamos la entrada del tweakbar
    vec3 reflectDir = reflect(-LightDir, Normal);
    vec3 halfwayDir = normalize(LightDir + ViewDir);
    //Material with lightDir components
    vec3 ambient  = /*ka */ dirLight.ambient;
    if(!dirLight.on)
        return ambient;
    vec3 diffuse  = diffuseColorK * dirLight.diffuse * max(0.0, dot(Normal, LightDir));
    vec3 specular = /*ks */ dirLight.specular * pow(max(0.0, dot(reflectDir, halfwayDir)), n);

    return ambient + diffuse + specular;
}

//vec3 intensityPointLight(PointLight pointLight, vec3 normal, vec3 ViewDir, vec3 diffuseColorK)
//{
//
//    vec3 lightDir = normalize(pointLight.position - fragPos);
//    vec3 R = reflect(-lightDir, normal);
//    vec3 halfwayDir = normalize(lightDir + ViewDir);
//    
//    vec3 ambient  = ka * pointLight.ambientColor;
//    if(!pointLight.on)
//        return ambient;
//    vec3 diffuse  = diffuseColorK * pointLight.diffuseColor * max(0.0, dot(normal, lightDir));
//    vec3 specular = ks * pointLight.specularColor * pow(max(0.0, dot(R, halfwayDir)), n);
//    
//    float dist = length(pointLight.position - fragPos);
//    float attenuation = 1.0f / (pointLight.attenuationK.x 
//        + pointLight.attenuationK.y * dist
//        + pointLight.attenuationK.z * (dist * dist));
//    
//    ambient  *= attenuation;  
//    diffuse   *= attenuation;
//    specular *= attenuation;  
//    
//    return ambient + diffuse + specular;
//}

void main()
{
    //Datos de vital importancia para todos
    vec3 normal = normalize(Normal);
    vec3 ViewDir = normalize(viewPos - fragPos.xyz);
    
    vec3 result;
//    if(!albedo)
//    {
////          result = intensiyLightDir(normal,ViewDir, kd);
////        result += intensityPointLight(pointLights[0], normal, ViewDir, kd);
////        result += intensityPointLight(pointLights[1], normal, ViewDir, kd);
//
//    }else {
//        result = intensiyLightDir(normal,ViewDir, texture2D(text, texCoord).rgb);
////        result += intensityPointLight(pointLights[0], normal, ViewDir, texture2D(text, texCoord).rgb);
////        result += intensityPointLight(pointLights[1], normal, ViewDir, texture2D(text, texCoord).rgb);
//        
//        if(texture2D(text, texCoord).a < 0.1)
//            discard;
//    }
//    result = intensiyLightDir(normal, ViewDir, texture2D(text, texCoord).rgb);
    result = intensiyLightDir(normal, ViewDir, vec3(1.0f,0.0f,1.0f));
    if(texture2D(text, texCoord).a < 0.1)
        discard;
    color = vec4(result, 1.0f);
    
    //Texture
    // color = color /* texture2D(text, texCoord).rgb*/;
    // color = vColor;
}