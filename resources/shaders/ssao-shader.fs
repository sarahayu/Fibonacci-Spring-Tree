#version 330 core
out vec4 FragColor;

uniform sampler2D posTex;
uniform sampler2D normalTex;
uniform sampler2D noiseTex;

uniform vec3 samples[36];
uniform mat4 projection;
uniform vec2 windowSize;
uniform float radius;

in vec2 v_texturePos;

int kernelSize = 36;
float bias = 0.1;

void main()
{
	vec2 noiseScale = vec2(windowSize.x/4.0, windowSize.y/4.0); 
	
	vec3 position = texture(posTex, v_texturePos).xyz;
	vec3 normal = normalize(texture(normalTex, v_texturePos).xyz);
	vec3 randomVec = texture(noiseTex, v_texturePos * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; i++)
	{
		vec3 ssample = TBN * samples[i];
		ssample = position + ssample * radius; 
		
		vec4 offset = vec4(ssample, 1.0);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz  = offset.xyz * 0.5 + 0.5;
		
		float sampleDepth = texture(posTex, offset.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
		occlusion += (sampleDepth >= ssample.z + bias ? 1.0 : 0.0) * rangeCheck;
	}  

	occlusion = 1.0 - (occlusion / kernelSize);
	FragColor = vec4(vec3(occlusion), 1.0);
}