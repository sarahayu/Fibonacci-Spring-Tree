#version 330 core
out vec4 FragColor;

uniform sampler2D screenTexture;
//uniform float mode;

varying vec2 v_texturePos;

void main()
{
	vec4 color = texture(screenTexture, v_texturePos).rgba;
	//vec4 color = vec4(0.96, 0.84, 0.65, 1.0);

	//if (mode != 2.0)
	//{
	//	color = texture(screenTexture, v_texturePos).rgba;
		
	//	if (mode == 1.0 && gl_FragCoord.y <= 200) 
	//	{
	//		if (color.a < 0.1) color.rgb = vec3(0.0,0.0,0.5);
	//		else color.rgb *= vec3(0.0,0.0,0.5);
	//	}
	//}

	FragColor = color;
}