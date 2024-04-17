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
	FragColor=vec4(1.0f,0.0f,1.0f,1.0f);
}
