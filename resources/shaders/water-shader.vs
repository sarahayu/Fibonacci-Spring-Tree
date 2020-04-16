#version 330 core
layout (location = 0) in vec3 l_pos;

uniform mat4 projView;
uniform mat4 lightMVP;

varying vec3 v_pos;
varying vec4 v_lightSpacePos;

void main()
{
    gl_Position = projView * vec4(l_pos, 1.0);
	v_pos = l_pos;
	v_lightSpacePos = lightMVP * vec4(l_pos, 1.0);
}