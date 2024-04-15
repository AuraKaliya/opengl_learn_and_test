#version 330 core

out vec4 FragColor;

//in vec4 vertexColor;
//in vec2 texCord;

in vec3 Normal;
in vec3 FragPos ;


//默认对应纹理单元0
//uniform sampler2D textureWall;
//uniform sampler2D textureEx;


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
	//vec3 ambient=ambientStrength*lightColor;
	//vec3 ambient=lightColor * material.ambient;
	vec3 ambient= light.ambient * material.ambient;

	//漫反射
	vec3 norm =normalize(Normal);
	vec3 lightDir=normalize(lightPos  - FragPos );
	float diff =max(dot(norm,lightDir),0.0);
	//vec3 diffuse=diff*lightColor ;
	//vec3 diffuse=lightColor * (diff * material.diffuse) ;
	vec3 diffuse=light.diffuse * (diff * material.diffuse) ;


	//镜面光
	vec3 viewDir = normalize( viewPos - FragPos );
	//vec3 reflectDir = reflect( vec3(-lightDir.x,-lightDir.y,-lightDir.z) , norm);
	vec3 reflectDir =  reflect( -lightDir , vec3(0,1,0)  );
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess );
	//vec3 specular = lightColor * (spec * material.specular );
	vec3 specular = light.specular * (spec * material.specular );

	//+diffuse + specular 
	//vec3 result =( ambient)  * objectColor ;
	vec3 result = ambient +diffuse + specular;
	
	FragColor=vec4(result,1.0f);


    //FragColor=sun* vec4(1.0f, 0.6f, 0.4f, 1.0f);
    //FragColor=vec4(ourColor2.xyz,1.0f);
    //FragColor=texture(texture0,texCord)*vec4(ourColor2.xyz,0.5f);
    //FragColor=mix(texture(textureWall,texCord),vec4(ourColor2.xyz,1.0f),0.6);
    //FragColor=mix(texture(textureWall,texCord),texture(textureEx,texCord),0.5);
}
