#version 330 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 billBoarSize;

void main() {
    
    mat4 VP = projection * view;
    vec3 CameraRight = vec3(vMatrix[0][0], vMatrix[1][0], vMatrix[2][0]);
    vec3 CameraUp = vec3(vMatrix[0][1], vMatrix[1][1], vMatrix[2][1]);

    vec3 Pos = gl_in[0].gl_Position.xyz;  // The desired point for the billboard

    vec4 v1 = VP * vec4(Pos + CameraRight * 0.5 * Size + CameraUp * -0.5 * Size, 1.0);
    vec4 v2 = VP * vec4(Pos + CameraRight * 0.5 * Size + CameraUp * 0.5 * Size, 1.0);
    vec4 v3 = VP * vec4(Pos + CameraRight * -0.5 * Size + CameraUp * -0.5 * Size, 1.0);
    vec4 v4 = VP * vec4(Pos + CameraRight * -0.5 * Size + CameraUp * 0.5 * Size, 1.0);

    
}