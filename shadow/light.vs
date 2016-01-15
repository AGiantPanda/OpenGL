#version 440 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) uniform mat4 model;
layout(location = 4) uniform mat4 view;
layout(location = 5) uniform mat4 projection;
layout(location = 6) uniform mat4 lightSpaceMatrix;
out vec3 normal;
out vec3 fragPos;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

void main()
{
	gl_Position = projection*view*model*vec4(vertexPosition, 1.0f);
	fragPos = vec3(model*vec4(vertexPosition,1.0f));
	normal = mat3(transpose(inverse(model)))*vertexNormal;
	TexCoords = texCoords;
	FragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0f);
}