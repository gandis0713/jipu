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
    mat4 orientation;
}
ubo;

void main()
{
    vec4 position = ubo.orientation * vec4(inPosition, 1.0f);
    gl_Position = ubo.mvp.proj * ubo.mvp.view * ubo.mvp.model * vec4(position.xyz, 1.0);
    outColor = inColor;
}