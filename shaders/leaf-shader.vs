#version 330 core
layout (location = 0) in vec3 l_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 v_texturePos;

void main()
{
	vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 position = vec3(0.0) 
		+ cameraRight * l_pos.x
		+ vec3(0.0,1.0,0.0) * l_pos.y;
	vec4 fragPos = model * vec4(position, 1.0);
    gl_Position = projection * view * fragPos;
	v_texturePos = l_pos + vec3(0.5,0.5,0.0);
}