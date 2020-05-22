#version 330 core
layout (location = 0) in vec2 l_pos;

out vec2 v_texturePos;

void main()
{
    gl_Position = vec4(l_pos.x, l_pos.y, 0.0, 1.0); 
    v_texturePos = (vec2(l_pos.x, l_pos.y) + vec2(1.0,1.0)) / 2.0;
}