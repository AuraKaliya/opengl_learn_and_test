#version 330 core
out vec4 FragColor;
in vec4 vertexColor;

in vec2 texCord;
uniform vec4 ourColor;
//默认对应纹理单元0
uniform sampler2D textureWall;
uniform sampler2D textureEx;

void main()
{
    //FragColor= vec4(1.0f, 0.6f, 0.4f, 1.0f);
   // FragColor=vec4(ourColor2.xyz,1.0f);
    //FragColor=texture(texture0,texCord)*vec4(ourColor2.xyz,0.5f);
    //FragColor=mix(texture(textureWall,texCord),vec4(ourColor2.xyz,1.0f),0.6);
    FragColor=mix(texture(textureWall,texCord),texture(textureEx,texCord),0.5);
}
