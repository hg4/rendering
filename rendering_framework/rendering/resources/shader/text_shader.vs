#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    vec4 clipPos = projection * vec4(aPos.xyz, 1.0);
	gl_Position = vec4(clipPos.xy,-clipPos.w,clipPos.w);
    TexCoords = aTexCoords;
}