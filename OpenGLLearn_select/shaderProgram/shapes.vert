#version 330 core
layout (location = 0) in vec3 aPos;
layout (location=1) in vec3 aNormal;

out vec3 ourColor2;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection ;
uniform mat4 scale ;

uniform mat4 mvp ;



void main()
{
	//projection*view*model*
   gl_Position = mvp*vec4(aPos.x,aPos.y,aPos.z,1.0);
    FragPos=vec3(model*vec4(aPos,1.0));
    vec4 tmpNormal=model*vec4(aNormal,0) ;
    Normal =tmpNormal.xyz;
}
