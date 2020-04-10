#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;

uniform vec3 cameraPos;
uniform vec3 lightSource;

varying vec2 v_texturePos;
varying float v_darken;
varying vec3 v_pos;
varying vec3 v_normal;

vec4 skyColor = vec4(0.96, 0.84, 0.65, 1.0);

void main()
{
	vec4 color = texture(leafTexture, v_texturePos);
	if (color.a < 0.1) discard;

    float ambientStrength = 0.5;
	vec3 lightColor = vec3(0.96, 0.84, 0.65);
    vec3 ambient = ambientStrength * lightColor;
	vec3 normal = normalize(v_normal);
	//if (!gl_FrontFacing) normal.xz = -normal.xz;
	vec3 lightDir = normalize(-lightSource);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;
	vec3 viewDir = normalize(cameraPos - v_pos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 32) * 0.5 * lightColor;

	color.xyz = (ambient + diffuse + specular) * color.xyz;
	color.xyz *= clamp(v_darken, 0.01, 1.0);
	color.xyz = mix(color.xyz, skyColor.xyz, clamp((gl_FragCoord.z / gl_FragCoord.w - 70.f) / 100.0, 0.0, 1.0));
	FragColor = color;
} 