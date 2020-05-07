#version 330 core
layout (location = 0) in vec3 l_pos;

uniform mat4 lightMVP;

varying vec2 v_texCoord;

void main()
{
	gl_Position = lightMVP * vec4(l_pos, 1.0);
	v_texCoord = vec2(0.0);
}