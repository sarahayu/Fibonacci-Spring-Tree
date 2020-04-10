#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 1) in vec3 l_normal;

uniform mat4 projView;
uniform mat4 lightMVP;

varying vec3 v_pos;
varying vec3 v_normal;
varying vec4 v_lightSpacePos;

void main()
{
    gl_Position = projView * vec4(l_pos, 1.0);
	
	v_pos = l_pos;
	v_normal = /*mat3(transpose(inverse(model))) **/ l_normal;
	v_lightSpacePos = lightMVP * vec4(l_pos, 1.0);
}