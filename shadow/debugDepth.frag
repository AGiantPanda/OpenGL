#version 440 core
in vec2 TexCoords;
out vec3 color;

uniform sampler2D depthMap;

void main()
{
	float depthValue = texture(depthMap, TexCoords).r;
	color = vec3(depthValue);
}