#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos ;

uniform vec3 lightPos;
uniform vec3 viewPos ;
uniform Material material;
uniform Light light;

void main()
{
	vec3 lightColor =vec3(1.0f,0.6f,0.8f);
	vec3 objectColor =vec3(0.4f,0.4f,0.6f);
	float ambientStrength=0.1;

	//环境光
	vec3 ambient= light.ambient * material.ambient;

	//漫反射
	vec3 norm =normalize(Normal);
	vec3 lightDir=normalize(lightPos  - FragPos );
	float diff =max(dot(norm,lightDir),0.0);
	vec3 diffuse=light.diffuse * (diff * material.diffuse) ;

	vec3 result = ambient +diffuse ;
	
	FragColor=vec4(result,1.0f);
}
