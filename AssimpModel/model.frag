#version 430 core

in vec2 TexCoords;

out vec4 color;

unifrom sampler2D texture_diffuse1;

void main()
{
	color = vec4(texture(texture_diffuse1, TexCoords));
}