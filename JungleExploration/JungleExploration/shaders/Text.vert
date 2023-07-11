#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 coord;
layout (location = 2) in vec3 color;

layout (location = 0) out vec2 outCoord;
layout (location = 1) out vec3 outColor;

void main(void)
{
    outCoord = coord;
    outColor = color;
    gl_Position = vec4(position, 0.5f, 1.0);
}