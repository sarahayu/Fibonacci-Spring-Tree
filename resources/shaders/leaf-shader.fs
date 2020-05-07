#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;
uniform sampler2D shadowMap;

uniform vec3 cameraPos;
uniform vec3 lightSource;

varying vec2 v_texturePos;
varying float v_darken;
varying vec3 v_pos;
varying vec3 v_normal;
varying vec4 v_lightSpacePos;

vec4 skyColor = vec4(0.96, 0.84, 0.65, 1.0);

float getShadow(vec3 position, vec3 lightDir)
{
	vec3 projCoords = v_lightSpacePos.xyz / v_lightSpacePos.w;
	projCoords = projCoords * 0.5 + vec3(0.5);
	if (projCoords.z > 1.0) return 0.0;
	float posDepth = projCoords.z;
	float bias = max(0.005, 0.05 * (1.0 - dot(v_normal, lightDir)));

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

void main()
{
	vec4 color = texture(leafTexture, v_texturePos);
	if (color.a != 1.0) discard;

    float ambientStrength = 0.5;
	vec3 lightColor = vec3(0.96, 0.84, 0.65);
    vec3 ambient = ambientStrength * lightColor;
	vec3 normal = normalize(v_normal);
	//if (!gl_FrontFacing) normal.xz = -normal.xz;
	vec3 lightDir = normalize(lightSource);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;
	vec3 viewDir = normalize(cameraPos - v_pos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 32) * 0.5 * lightColor;

	color.rgb = (ambient + (diffuse + specular) * (1.0 - getShadow(v_pos, lightDir))) * color.rgb;
	color.rgb *= clamp(v_darken, 0.01, 1.0);
	//color.rgb = mix(color.rgb, skyColor.rgb, clamp((gl_FragCoord.z / gl_FragCoord.w - 70.f) / 100.0, 0.0, 1.0));
	//color.rgb = vec3(1.0 - getShadow(vec3(v_pos.x, 0.f, v_pos.z), lightDir));
	FragColor = color;
} 