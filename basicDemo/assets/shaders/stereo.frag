#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
in vec2 vText;
uniform int left;
uniform int hasText;
uniform sampler2D text;

// Fragment Color
out vec4 color;

void main()
{

	vec3 Color;
	if(hasText == 1){
		Color = texture(text, vText).rgb;
	}else{
		Color = vColor;

	}
	if(left == 1){
		color = vec4(0, Color.g, Color.b, 1.0f);
	}else{
		color = vec4(Color.r, 0, 0, 1.0f);
	}
}