#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 1) in vec3 l_normal;
layout (location = 4) in mat4 l_model;

uniform mat4 projView;
uniform mat4 lightMVP;

varying vec3 v_pos;
varying vec3 v_normal;
varying vec4 v_lightSpacePos;

void main()
{	
	v_pos = (l_model * vec4(l_pos, 1.0)).xyz;
    gl_Position = projView * vec4(v_pos, 1.0);
	v_normal = mat3(transpose(inverse(l_model))) * l_normal;
	//v_normal = (l_model * vec4(l_normal, 1.0)).xyz;
	v_lightSpacePos = lightMVP * vec4(v_pos, 1.0);
}