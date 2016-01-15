#version 440
struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 fragColor;
in  vec3 fragPos;
in  vec3 normal;
in  vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

float LinearizeDepth(float depth)
{
	float near = 0.1;
	float far = 100.0;
	float z = depth * 2.0 - 1.0;
	return (2.0 * near) / (far + near - z * (far - near));
}

void main()
{
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	
	if (gl_FrontFacing){
		vec3 norm = normalize(normal);
		vec3 lightDir = normalize(light.position - fragPos);
		
		//this can realise a cartoon effect, set the box to a solid color for better look and cancle the specular
		float ani = max(dot(norm, lightDir), 0);
		if (ani >= 0.75)
			ani = 0.75;
		else if (ani >= 0.5)
			ani = 0.5;
		else if (ani >= 0.25)
			ani = 0.25;
		else 
			ani = 0;

		float diff = max(dot(norm, lightDir), 0);
		vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

		vec3 result = ambient + diffuse + specular;

		float depth = LinearizeDepth(gl_FragCoord.z);
		fragColor = vec4(result, 1.0f);
	}
	else{
		//sometimes we dont need to cull the back face, but we need to change the normal of that face,
		vec3 norm = normalize(-normal);
		vec3 lightDir = normalize(light.position - fragPos);
		float diff = max(dot(norm, lightDir), 0);
		vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

		vec3 result = ambient + diffuse + specular;

		float depth = LinearizeDepth(gl_FragCoord.z);
		fragColor = vec4(result, 1.0f);
	}
}