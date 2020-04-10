#version 330 core
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float cameraPitch;
uniform float screenHeight;

varying vec2 v_texturePos;

vec4 skyColor = vec4(0.96, 0.84, 0.65, 1.0);

vec4 fetchColor(sampler2D tex, vec2 pos)
{
	vec4 color = texture(tex, pos + vec2(0.0,-1.0/600));
	if (color.a < 0.1) color.rgba = skyColor;
	//color.rgb = vec3(1.0,0.0,0.0);
	return color;
}

void main()
{
	float screenHorizon = screenHeight / 2 * (1 - tan(cameraPitch));
	if (gl_FragCoord.y > screenHorizon) discard;

	const float offset = 1.0 / 500.0;  

    vec2 offsets[9] = vec2[](
        vec2(-1.0,  1.0),
        vec2( 0.0f,    1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0,  0.0f),  
        vec2( 0.0f,    0.0f),  
        vec2( 1.0,  0.0f),  
        vec2(-1.0, -1.0),
        vec2( 0.0f,   -1.0),
        vec2( 1.0, -1.0)   
    );

    float kernel[9] = float[](
		2.0 / 24, 2.0 / 24, 2.0 / 24,
		4.0 / 24, 4.0 / 24, 4.0 / 24,
		2.0 / 24, 2.0 / 24, 2.0 / 24  
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
        sampleTex[i] = fetchColor(screenTexture, v_texturePos.st + offsets[i] * offset).rgb;

    vec4 color = vec4(vec3(0.0), 1.0);
    for(int i = 0; i < 9; i++)
		color.rgb += sampleTex[i] * kernel[i];

	color *=  vec4(0.84, 0.92, 0.97, 1.0);
	color.rgb *= clamp(min(screenHorizon - gl_FragCoord.y, 30) / 30, 0.7, 1.0);
	color.a *= clamp(min(screenHorizon - gl_FragCoord.y, 50) / 50, 0.0, 1.0);

	FragColor = color;

}