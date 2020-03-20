#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 1) in vec3 l_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

varying vec3 v_fragPos;
varying vec3 v_normal;
varying vec3 v_cameraPos;

void main()
{
	vec4 fragPos = model * vec4(l_pos, 1.0);
    gl_Position = projection * view * fragPos;
	v_fragPos = vec3(fragPos);
	v_normal = mat3(transpose(inverse(model))) * l_normal;
	v_cameraPos = cameraPos;
}