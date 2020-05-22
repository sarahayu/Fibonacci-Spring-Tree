#version 330 core
out vec4 FragColor;

in vec3 v_texturePos;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, v_texturePos);
	//FragColor = vec4(1.0,0.0,0.0,1.0);
}