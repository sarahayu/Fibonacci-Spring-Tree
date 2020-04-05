#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;

varying vec2 v_texturePos;
varying float v_darken;

void main()
{
	vec3 lightColor = vec3(0.96, 0.84, 0.65);
	vec4 color = texture(leafTexture, v_texturePos);
	if (color.a < 0.1) discard;
	color.xyz *= lightColor;
	color.xyz *= max(min(v_darken, 1.0),0.01);
	color.xyz = mix(color.xyz, vec3(0.0), (gl_FragCoord.z / gl_FragCoord.w - 40.0) / 40.0);
	FragColor = color;
} 