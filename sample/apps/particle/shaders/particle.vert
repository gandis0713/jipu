#version 450

layout(location = 0) in vec3 inPos;

void main()
{
    gl_PointSize = 14.0;
    gl_Position = vec4(inPos, 1.0);
}