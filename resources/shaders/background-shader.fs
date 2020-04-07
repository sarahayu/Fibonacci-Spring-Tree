#version 330 core
out vec4 FragColor;

uniform sampler2D screenTexture;

varying vec2 v_texturePos;

void main()
{
	FragColor = vec4(0.96, 0.84, 0.65, 1.0);
}