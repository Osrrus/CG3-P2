#version 330 core

layout (location = 0) in vec3 vertex;

layout (location = 1) in vec2 textureCoor;

out vec4 particleColor;
out vec2 uvs;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;
uniform vec2 billBoarSize;

void main() {
    
    uvs = textureCoor;
    particleColor = color;

    //vec3 cameraR = vec3(view[0][0],view[1][0],view[2][0]);
    //vec3 cameraL = vec3(view[0][1],view[1][1],view[2][1]);

    //vec4 billBoarCenter4 = model *vec4(vertex, 1.0);
    //vec3 billBoarCenter = vec3(billBoarCenter4.x/0.5f,billBoarCenter4.y/0.5f,billBoarCenter4.z/0.5f);

    //vec3 position = billBoarCenter +  cameraR * billBoarSize.x * vertex.x + cameraL * billBoarSize.y * vertex.y;

    //gl_Position = projection * view  *vec4(position, 1.0);

    gl_Position = projection * view  *model *vec4(vertex, 1.0);

    
}