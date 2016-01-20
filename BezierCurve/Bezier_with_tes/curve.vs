#version 440
layout (location = 0) in vec2 VertexPosition;
uniform mat4 MVP; //projection * view * model
void main()
{
	gl_Position = vec4(VertexPosition, 0.0, 1.0);
}