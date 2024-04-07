#version 330 core
layout (location = 0) in vec3 aPos ;
uniform vec3 lightPos ;

uniform mat4 model;
uniform mat4 view;

void main()
{
    //gl_Position = vec4(lightPos.xyz,1.0);
	gl_Position =  view*model*vec4( aPos.xyz,1.0);
}
