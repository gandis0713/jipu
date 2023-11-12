#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

struct MVP
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(binding = 0) uniform UBO
{
    MVP mvp;
}
ubo;

void main()
{
    gl_Position = vec4(inPosition, 1.0);
    outColor = inColor;
}