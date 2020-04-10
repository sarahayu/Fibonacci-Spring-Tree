#version 330 core
out vec4 FragColor;

uniform sampler2D screenTexture;

varying vec2 v_texturePos;

vec4 skyColor = vec4(0.96, 0.84, 0.65, 1.0);

void main()
{
	FragColor = skyColor;
}