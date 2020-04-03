#version 430 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vertexPosition;
// Atributte 1 of the vertex
layout (location = 1) in vec3 vertexNormal;
// Attribute 2 of the vertex
layout (location = 2) in vec2 vertexTexPos;

//layout (location = 3) in vec2 idRGB;
// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool invertedNormals = false;

// Vertex data out data
out vec3 vColor;
// Vertex normal
out vec3 vNor;
// Vertex texture position
out vec2 vTexPos;
out vec3 fragPos;

void main()
{
	vTexPos = vertexTexPos;
    vColor = vertexPosition;   
    vNor = vertexNormal;   
    fragPos = vec3(model * vec4(vertexPosition, 1.0f));
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    vNor = normalMatrix * (invertedNormals ? -vNor : vNor);
    gl_Position = projection * view * model * vec4((vertexPosition), 1.0f);
}