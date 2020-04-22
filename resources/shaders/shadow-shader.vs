#version 330 core
layout (location = 0) in vec3 l_pos;
layout (location = 2) in vec2 l_texCoord;
layout (location = 4) in mat4 l_model;

uniform mat4 lightMVP;

varying vec2 v_texCoord;

void main()
{
    gl_Position = lightMVP * l_model * vec4(l_pos, 1.0);
    v_texCoord = l_texCoord;
}