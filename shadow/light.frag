#version 440 core
struct Light{
	bool state;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	//for attenuation
	float constant;
	float linear;
	float quadratic;
	//for spotlight
	vec3 direction;
	float cutOff;
	float outerCutOff;
};

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

out vec4 fragColor;
in  vec3 fragPos;
in  vec3 normal;
in  vec2 TexCoords;
in  vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform Material material;
uniform sampler2D depthMap;
uniform Light fixedLight;
uniform Light direcLight;
uniform Light spotLight;

vec3 CalcGray(Light light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 CalcFixedLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 CalcDirecLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 CalcSpotLight_blinnphong(Light light, vec3 normal, vec3 viewDir, vec3 fragPos);
float LinearizeDepth(float depth);
float shadowcalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main()
{
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 result = vec3(0.0f, 0.0f, 0.0f);
	
	if(gl_FrontFacing)
		result += CalcFixedLight(fixedLight, normal, viewDir, fragPos);
	else{
		result += CalcFixedLight(fixedLight, -normal, viewDir, fragPos);
	}
	fragColor = vec4(result, 1.0);
}

vec3 CalcGray(Light light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 norm = normalize(normal);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear*distance + light.quadratic*distance*distance);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(0.5,0.5,0.5));
	vec3 ambient = light.ambient * vec3(0.5,0.5,0.5);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir),0.0),material.shininess);
	vec3 specular = light.specular*(spec*vec3(0.5,0.5,0.5));
	
	vec3 result = vec3(0.5,0.5,0.5);
	result = ambient;
	result += diffuse;
	result += specular;
	result *= attenuation;
	return result;
}

vec3 CalcDirecLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position);
	vec3 norm = normalize(normal);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir),0.0),material.shininess);
	vec3 specular = light.specular*spec*vec3(texture(material.specular, TexCoords));
	
	return (ambient+diffuse+specular);
}

vec3 CalcFixedLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos)
{	
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 norm = normalize(normal);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear*distance + light.quadratic*distance*distance);
	float shadow = shadowcalculation(FragPosLightSpace, norm, lightDir);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 halfwayDir = normalize(lightDir+viewDir);
	float spec = pow(max(dot(norm, halfwayDir),0.0),material.shininess);
	vec3 specular = light.specular*(spec*vec3(texture(material.diffuse, TexCoords)));
	
	return ambient+(1 - shadow) * (diffuse+specular);//*attenuation;
}

vec3 CalcSpotLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 norm = normalize(normal);
	float distance = length(light.position-fragPos);
	float attenuation = 1.0f / (light.constant+light.linear*distance+light.quadratic*distance*distance);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir),0.0),material.shininess);
	vec3 specular = light.specular*spec*vec3(texture(material.specular, TexCoords));
	
	return (ambient+(diffuse+specular)*intensity)*attenuation;
}

vec3 CalcSpotLight_blinnphong(Light light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 norm = normalize(normal);
	float distance = length(light.position-fragPos);
	float attenuation = 1.0f / (light.constant+light.linear*distance+light.quadratic*distance*distance);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
	vec3 halfwayDir = normalize(lightDir+viewDir);
	float spec = pow(max(dot(norm, halfwayDir),0.0),material.shininess);
	vec3 specular = light.specular*spec*vec3(texture(material.specular, TexCoords));
	
	return (ambient+(diffuse+specular)*intensity)*attenuation;
}

float LinearizeDepth(float depth)
{
	float near = 0.1;
	float far = 100.0;
	float z = depth * 2.0 - 1.0;
	return (2.0 * near) / (far + near - z * (far - near));
}

float shadowcalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	//when we output a clip-space vertex position to gl_Position in the vertex shader,
	//OpenGL automatically does a perspective divide e.g. transform clip-space coordinates in the range [-w, w] to [-1, 1] by dividing the x,y and z component by the vector;s w component,
	//this time we have to do it ourself.
//plus the depth from the depth map is in the range [0, 1] and we also need to normalize it to the range [0, 1].
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = 0.00005;//max(0.00005 * (1.0 - dot(normal, lightDir)), 0.00001);
	//it is not easy to choose a applicable bias
	//bias is neither too big to cause a shadow acne or too small to case a peter panning
	//use front face culling to solve the peter panning
	
	float shadow = 0.0;
	//here use PCF to reduce the aliasing
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; x++){
		for(int y = -1; y <= 1; y++){
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow /= 9.0;
	return shadow;
}