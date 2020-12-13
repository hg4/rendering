#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D brdfLUT;

void main()
{
    vec2 col = texture(brdfLUT, TexCoords).rg;
    FragColor = vec4(col,0.0,1.0);
} 