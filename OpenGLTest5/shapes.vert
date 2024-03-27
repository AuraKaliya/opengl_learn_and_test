#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location=1) in vec3 aColor;
layout (location=1) in vec2 aTexCord;

out vec4 vertexColor;
out vec2 texCord;
//uniform mat4 theMatrix;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   //gl_Position = theMatrix*vec4(aPos.x,aPos.y,aPos.z,1.0);
   gl_Position = projection*view*model*vec4(aPos.x,aPos.y,aPos.z,1.0);
   vertexColor=vec4(0.5,0.0,0.1,1.0);
   //ourColor2=aColor;
   texCord=aTexCord;
}
