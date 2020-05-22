#version 330 core
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D shadowMap;
uniform vec2 windowSize;

in vec3 v_pos;
in vec4 v_lightSpacePos;

vec4 skyColor = vec4(0.96, 0.84, 0.65, 1.0);

float getShadow(vec3 position)
{
	vec3 projCoords = v_lightSpacePos.xyz / v_lightSpacePos.w;
	projCoords = projCoords * 0.5 + vec3(0.5);
	if (projCoords.z > 1.0) return 0.0;
	float posDepth = projCoords.z;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
		{
			float shadowDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (posDepth - 0.005 > shadowDepth ? 1.0 : 0.0);
		}
	
	return shadow / 9.0;
}

vec4 fetchColor(sampler2D tex, vec2 pos)
{
	vec4 color = texture(tex, pos + vec2(0.0,-1.0/600));
	if (color.a < 0.1) color.rgba = skyColor;
	//color.rgb = vec3(1.0,0.0,0.0);
	return color;
}

void main()
{
	FragColor = vec4(0.0,0.412,0.58,0.5);
	FragColor = vec4(texture(screenTexture, gl_FragCoord.xy / textureSize(screenTexture, 0)).rgb + vec3(0.0,0.412,0.58) * 0.1 * (1.0 - getShadow(v_pos)), 1.0);
}