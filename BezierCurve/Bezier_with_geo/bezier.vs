#version 430 core
layout(location = 0) in vec3 vertexPosition;

uniform mat4 MVP;
uniform int num;

out VS_OUT{
	int num;
	mat4 MVP;
} vs_out;

void main()
{
	gl_Position = vec4(vertexPosition, 1.0f);
	vs_out.num = num;
	vs_out.MVP = MVP;
}