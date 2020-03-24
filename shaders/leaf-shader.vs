#version 330 core
layout (location = 0) in vec3 l_pos;

uniform mat4 localModel;
uniform mat4 globalModel;
uniform mat4 view;
uniform mat4 projection;

varying vec2 v_texturePos;
varying float v_darken;

void main()
{
	vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 position = vec3(0.0) 
		+ cameraRight * l_pos.x
		+ vec3(0.0,1.0,0.0) * l_pos.y;
	mat4 viewModel = view * globalModel;
	viewModel[0][0] = 1.0;
	viewModel[0][1] = 0.0;
	viewModel[0][2] = 0.0;
	viewModel[1][1] = 1.0;
	viewModel[2][0] = 0.0;
	viewModel[2][1] = 0.0;
	viewModel[2][2] = 1.0;

	viewModel = viewModel;
	//vieww = mat4(	vieww[0][0], vieww[0][1], vieww[0][2], vieww[0][3],
	//				vieww[1][0], vieww[1][1], vieww[1][2], vieww[1][3],
	//				vieww[2][0], vieww[2][1], vieww[2][2], vieww[2][3],
	//				vieww[3][0], vieww[3][1], vieww[3][2], vieww[3][3]);

    gl_Position = projection * viewModel * localModel * vec4(l_pos, 1.0);
	v_texturePos = -l_pos.xy + vec2(0.5,0.5);

	float scale = sqrt(localModel[0][0] * localModel[0][0] + localModel[0][1] * localModel[0][1] + localModel[0][2] * localModel[0][2]);
	vec3 localModelPos = vec3(localModel * vec4(l_pos, 1.0));
	localModelPos.y += scale / 2;
	v_darken = localModelPos.y / scale;
}