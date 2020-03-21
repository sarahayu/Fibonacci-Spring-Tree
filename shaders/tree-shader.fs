#version 330 core
out vec4 FragColor;

uniform vec3 cameraPos;

varying vec3 v_fragPos;
varying vec3 v_normal;

void main()
{
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(1.0);
	vec3 normal = normalize(v_normal);
	vec3 lightDir = normalize(vec3(-1.0));
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * vec3(1.0);
	vec3 viewDir = normalize(cameraPos - v_fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 32) * 0.5 * vec3(1.0);

    FragColor = vec4((ambient + diffuse + specular) * vec3(54.f / 255, 26.f / 255, 13.f / 255), 1.f);
} 