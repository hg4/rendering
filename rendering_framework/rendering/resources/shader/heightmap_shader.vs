#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


out vec3 WorldPos;
out vec4 FragPosInLightSpace;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TtoW;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 LIGHTSPACE_MATRIX_VP;
void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));
	FragPosInLightSpace = LIGHTSPACE_MATRIX_VP * vec4(WorldPos,1.0);
	mat3 temp = transpose(inverse(mat3(model)));
    Normal = normalize(temp * aNormal);
    TexCoords = aTexCoords;
    TtoW = mat3(normalize(temp * aTangent),normalize(temp * aBitangent),normalize(temp * aNormal));
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}