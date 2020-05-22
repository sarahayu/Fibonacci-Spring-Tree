#version 330 core
layout (location = 0) out vec4 Position;
layout (location = 1) out vec4 Normal;

uniform sampler2D tex;

in vec3 v_pos;
in vec3 v_normal;
in vec2 v_texCoord;

void main()
{    
	if (texture(tex, v_texCoord).a < 0.1) discard;
    Position = vec4(v_pos, 1.0);
    Normal = vec4(normalize(v_normal), 1.0);
	//if (!gl_FrontFacing) Normal.xz = -Normal.xz;
}  