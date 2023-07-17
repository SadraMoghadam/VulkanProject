#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inCoord;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec2 outCoord;
layout(location = 1) out vec3 outColor;

void main()
{
    gl_Position = vec4(inPos, 0.0, 1.0);
    outCoord = inCoord;
    outColor = inColor;
}

