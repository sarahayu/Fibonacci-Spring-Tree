#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;

varying vec2 v_texturePos;
varying float v_darken;

void main()
{
	vec3 lightColor = vec3(0.96, 0.84, 0.65);
	//vec3 lightColor = vec3(255.0 / 255, 198.0 / 255, 140.0 / 255);
    vec4 color = texture(leafTexture, v_texturePos);
	if (color.a < 0.1) discard;
	//color.a = ;
	color.xyz *= lightColor;
	color.xyz *= max(min(v_darken + 0.1, 1.0),0.1);
	color.xyz = mix(color.xyz, vec3(0.0), (gl_FragCoord.z / gl_FragCoord.w - 40.0) / 40.0);
	FragColor = color;
} 