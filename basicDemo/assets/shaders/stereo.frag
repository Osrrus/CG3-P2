#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
uniform int left;

// Fragment Color
out vec4 color;

void main()
{
	if(left == 1){
		color = vec4(0, vColor.g, vColor.b, 1.0f);
	}else{
		color = vec4(vColor.r, 0, 0, 1.0f);
	}
}