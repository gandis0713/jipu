#version 450

layout(location = 0) out vec4 outColor1;
layout(location = 1) out vec4 outColor2;

void main()
{
    outColor1 = vec4(1.0, 1.0, 0.0, 1.0);
    outColor2 = vec4(1.0, 0.0, 0.0, 1.0);
}