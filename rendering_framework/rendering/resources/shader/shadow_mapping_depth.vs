#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 LIGHTSPACE_MATRIX_VP;
uniform mat4 model;

void main()
{
    gl_Position = LIGHTSPACE_MATRIX_VP * model * vec4(aPos, 1.0);
}