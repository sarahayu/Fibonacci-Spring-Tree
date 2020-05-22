#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 1) in vec3 l_normal;
layout (location = 2) in uint l_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightMVP;

varying vec3 v_pos;
varying vec3 v_normal;
varying vec4 v_lightSpacePos;
varying vec3 v_color;
flat varying uint v_colorOffset;

const vec3 colors[2] = {vec3(54.f / 255, 26.f / 255, 13.f / 255), vec3(0.23, 0.25, 0.28)};

void main()
{
    gl_Position = projection * view * vec4(l_pos, 1.0);
	gl_ClipDistance[0] = dot( vec4(l_pos,1.0), vec4(0.0,1.0,0.0,-2.9));
	
	v_pos = l_pos;
	v_normal = l_normal;
	v_lightSpacePos = lightMVP * vec4(l_pos, 1.0);
	v_color = colors[l_color];
	v_colorOffset = l_color;
}