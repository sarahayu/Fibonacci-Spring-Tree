#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 1) in vec3 l_normal;
layout (location = 2) in vec2 l_texCoord;;
layout (location = 3) in float l_yCenter;
layout (location = 4) in mat4 l_model;

uniform mat4 projView;
uniform float time;
uniform float leafSize;
uniform mat4 lightMVP;

varying vec2 v_texturePos;
varying float v_darken;
varying vec3 v_pos;
varying vec3 v_normal;
varying vec4 v_lightSpacePos;

void main()
{
	v_pos = (l_model * vec4(l_pos, 1.0)).xyz;
	v_darken = (v_pos.y - (l_model[3][1] - leafSize * 1.41/2)) / (leafSize * 1.41);
	vec3 sinPos = v_pos;
	sinPos.y += sin(time * 2 + v_pos.x / 10) / 5;

	gl_Position = projView * vec4(sinPos, 1.0);
	v_texturePos = l_texCoord;
	v_normal = mat3(transpose(inverse(l_model))) * l_normal;
	v_lightSpacePos = lightMVP * vec4(v_pos, 1.0);
}