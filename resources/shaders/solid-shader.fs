#version 330 core
out vec4 FragColor;

uniform sampler2D shadowMap;
uniform sampler2D ssaoTexture;
uniform vec3 cameraPos;
uniform vec3 lightSource;
uniform bool useShadows;
uniform bool useSSAO;
uniform bool useLighting;

in vec3 v_pos;
in vec3 v_normal;
in vec4 v_lightSpacePos;
in vec3 v_color;
flat in uint v_colorOffset;

vec4 skyColor = vec4(0.96, 0.84, 0.65, 1.0);
vec3 lightColor = vec3(0.96, 0.84, 0.65);

float getShadow(vec3 position, vec3 lightDir)
{
	vec3 projCoords = v_lightSpacePos.xyz / v_lightSpacePos.w;
	projCoords = projCoords * 0.5 + vec3(0.5);
	if (projCoords.z > 1.0) return 0.0;
	float posDepth = projCoords.z;
	float bias = max(0.001, 0.02 * (1.0 - dot(v_normal, lightDir)));

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
		{
			float shadowDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (posDepth - bias > shadowDepth ? 1.0 : 0.0);
		}
	
	return shadow / 9.0;
}

vec3 getLighting()
{
	if (!useLighting) return vec3(1.0);
    float ambientStrength = useSSAO ? texture(ssaoTexture, gl_FragCoord.xy / textureSize(ssaoTexture, 0)).r : 1.0;
    vec3 ambient = lightColor * 0.4 * ambientStrength;
	vec3 normal = normalize(v_normal);
	vec3 lightDir = normalize(lightSource);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor * 0.9;
	float shadow = useShadows ? (1.0 - getShadow(v_pos, lightDir)) : 1.0;

	return ambient + diffuse * shadow;
}

void main()
{
	// quick messy way of adding snow to stone
	vec3 col = v_color;
	if (v_colorOffset == uint(1)) col = mix(col, vec3(1.0), clamp((v_pos.y - 10) / 20, 0.0,1.0));

    vec4 color = vec4(getLighting() * col, 1.f);
	FragColor = color;
} 