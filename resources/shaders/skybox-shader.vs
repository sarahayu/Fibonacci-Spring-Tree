#version 330 core
layout (location = 0) in vec3 l_pos;

varying vec3 v_texturePos;

uniform mat4 projection;
uniform mat4 view;
uniform mat3 sunRotate;

void main()
{
    v_texturePos = sunRotate * l_pos;
    vec4 pos = projection * view * vec4(l_pos, 1.0);
    gl_Position = pos.xyww;
}  