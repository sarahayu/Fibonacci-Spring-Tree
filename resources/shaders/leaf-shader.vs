#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 1) in vec3 l_normal;
layout (location = 2) in vec2 l_texCoord;;
layout (location = 3) in float l_yCenter;

uniform mat4 projView;
uniform float time;
uniform float leafSize;

varying vec2 v_texturePos;
varying float v_darken;
varying vec3 v_pos;
varying vec3 v_normal;

void main()
{
	v_darken =  (l_pos.y - (l_yCenter - leafSize * 1.41/2)) / (leafSize * 1.41);

	vec3 sinPos = l_pos;
	sinPos.y += sin(time * 2 + l_pos.x / 10) / 5;

	gl_Position = projView * vec4(sinPos, 1.0);
	v_texturePos = l_texCoord;
	v_pos = l_pos;
	v_normal = l_normal;
}