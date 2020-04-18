#version 330 core

uniform sampler2D tex;

varying vec2 v_texCoord;

void main()
{
	if (texture(tex, v_texCoord).a < 0.1) discard;
}