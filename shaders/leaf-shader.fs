#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;

varying vec3 v_texturePos;

void main()
{
    vec4 color = texture(leafTexture, v_texturePos.xy);
	if (color.a < 0.1) discard;
	FragColor = color;
} 