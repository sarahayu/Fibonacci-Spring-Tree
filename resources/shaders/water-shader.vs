#version 330 core
layout (location = 0) in vec3 l_pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightMVP;

out vec3 v_pos;
out vec4 v_lightSpacePos;

void main()
{
    gl_Position = projection * view * vec4(l_pos, 1.0);
	v_pos = l_pos;
	v_lightSpacePos = lightMVP * vec4(l_pos, 1.0);
}