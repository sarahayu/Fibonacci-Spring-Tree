#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 1) in vec3 l_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 v_fragPos;
varying vec3 v_normal;

void main()
{
	vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);
	vec3 position = vec3(0.0) 
		+ cameraRight * l_pos.x
		+ cameraUp * l_pos.y;
	vec4 fragPos = model * vec4(l_pos, 1.0);
    gl_Position = projection * view * fragPos;
	v_fragPos = vec3(fragPos);
	v_normal = mat3(transpose(inverse(model))) * l_normal;
}