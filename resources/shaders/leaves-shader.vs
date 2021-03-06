#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 1) in vec3 l_normal;
layout (location = 2) in vec2 l_texCoord;;
layout (location = 3) in float l_yCenter;

uniform mat4 projection;
uniform mat4 view;
uniform float time;
uniform float leafSize;
uniform mat4 lightMVP;

out vec2 v_texturePos;
out float v_darken;
out vec3 v_pos;
out vec3 v_normal;
out vec4 v_lightSpacePos;

void main()
{
	v_darken =  (l_pos.y - (l_yCenter - leafSize * 1.41/2)) / (leafSize * 1.41);

	vec3 sinPos = l_pos;
	float timeAdjusted = time + sin(time / 2);
	sinPos.y += sin(time * 2 + l_pos.x / 10) / 10;
	sinPos.x += sin(timeAdjusted * 5 + (l_pos.z + l_pos.y) / 2) / 20;
	sinPos.z += sin(timeAdjusted * 5 + (l_pos.x + l_pos.y) / 2) / 20;

	gl_Position = projection * view * vec4(sinPos, 1.0);
	gl_ClipDistance[0] = dot( vec4(l_pos,1.0), vec4(0.0,1.0,0.0,-2.0));

	v_texturePos = l_texCoord;
	v_pos = l_pos;
	v_normal = l_normal;
	v_lightSpacePos = lightMVP * vec4(l_pos, 1.0);
}