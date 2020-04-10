#version 330 core
out vec4 FragColor;

uniform sampler2D shadowMap;
uniform vec3 cameraPos;
uniform vec3 lightSource;

varying vec3 v_pos;
varying vec3 v_normal;
varying vec4 v_lightSpacePos;

vec4 skyColor = vec4(0.96, 0.84, 0.65, 1.0);

float getShadow(vec3 position, vec3 lightDir)
{
	vec3 projCoords = v_lightSpacePos.xyz / v_lightSpacePos.w;
	projCoords = projCoords * 0.5 + vec3(0.5);
	float shadowDepth = texture(shadowMap, projCoords.xy).r;
	float posDepth = projCoords.z;
	float bias = max(0.005, 0.5 * (1.0 - dot(v_normal, lightDir)));
	
	return (posDepth - 0.005 > shadowDepth ? 1.0 : 0.0);
}

void main()
{
    float ambientStrength = 0.3;
	vec3 lightColor = vec3(0.96, 0.84, 0.65);
    vec3 ambient = ambientStrength * lightColor;
	vec3 normal = normalize(v_normal);
	vec3 lightDir = normalize(-lightSource);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;
	vec3 viewDir = normalize(cameraPos - v_pos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 32) * 0.5 * lightColor;

    vec4 color = vec4((ambient + (diffuse + specular) * (1.0 - getShadow(v_pos, lightDir))) * vec3(54.f / 255, 26.f / 255, 13.f / 255), 1.f);
	color.xyz = mix(color.xyz, skyColor.xyz, clamp((gl_FragCoord.z / gl_FragCoord.w - 70.f) / 100.0, 0.0, 1.0));
	FragColor = color;
} 