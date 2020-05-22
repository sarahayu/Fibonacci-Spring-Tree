#version 330 core
layout(location = 0) in vec3 l_pos;
layout(location = 1) in vec3 l_normal;

uniform mat4 view;
uniform mat3 invView;
uniform mat4 projection;

out vec3 v_pos;
out vec3 v_normal;
out vec2 v_texCoord;

void main()
{
	v_pos = (view * vec4(l_pos, 1.0)).xyz;

	gl_Position = projection * vec4(v_pos, 1.0);
	gl_ClipDistance[0] = dot( vec4(l_pos,1.0), vec4(0.0,1.0,0.0,-2.0));

	v_normal = invView * l_normal;
	v_texCoord = vec2(0.0);
}