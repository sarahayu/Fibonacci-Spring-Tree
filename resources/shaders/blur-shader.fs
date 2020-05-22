#version 330 core
out vec4 FragColor;
  
in vec2 v_texturePos;
  
uniform sampler2D ssaoInput;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -1; x < 1; ++x) 
    {
        for (int y = -1; y < 1; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, v_texturePos + offset).r;
        }
    }
    FragColor = vec4(vec3(result / (2.0 * 2.0)), 1.0);
}  