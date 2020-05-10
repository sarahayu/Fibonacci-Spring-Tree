#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 1) in vec3 l_normal;
layout (location = 2) in vec2 l_texCoord;;

uniform mat4 view;
uniform mat3 invView;
uniform mat4 projection;
uniform float time;

varying vec3 v_pos;
varying vec3 v_normal;
varying vec2 v_texCoord;

void main()
{
	vec3 sinPos = l_pos;
	float timeAdjusted = time + sin(time / 2);
	sinPos.y += sin(time * 2 + l_pos.x / 10) / 10;
	sinPos.x += sin(timeAdjusted * 5 + (l_pos.z + l_pos.y) / 2) / 20;
	sinPos.z += sin(timeAdjusted * 5 + (l_pos.x + l_pos.y) / 2) / 20;

	v_pos = (view * vec4(sinPos, 1.0)).xyz;

    gl_Position = projection * vec4(v_pos, 1.0);
	
	v_normal = invView * l_normal;
	v_texCoord = l_texCoord;
}