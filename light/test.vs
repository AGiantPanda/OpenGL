#version 440
layout(location = 0) in vec3 vertexPosition;   //attribute 支持    location显式指定
layout(location = 1) in vec3 vertexNormal;
layout(location = 5) in vec2 texCoords;
layout(location = 2) uniform mat4 model;       //uniform支持 location显式指定在glsl4.3
layout(location = 3) uniform mat4 view;
layout(location = 4) uniform mat4 projection;
layout(location = 6) uniform mat4 rotate;
layout(location = 7) uniform mat4 invrotate;
out vec3 normal;
out vec3 fragPos;
out vec2 TexCoords;

void main()
{
	gl_Position = projection*view*model*rotate*vec4(vertexPosition, 1.0f);
	vec4 fragPos4 = rotate*vec4(vertexPosition, 1.0f);
	fragPos = fragPos4.xyz;
	vec4 normal4 = invrotate*vec4(vertexNormal, 1.0f);
	normal = normal4.xyz;
	TexCoords = texCoords;
}