#version 450

layout(location = 0) in vec3 inPosition;

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
    gl_Position = ubo.mvp.proj * ubo.mvp.view * ubo.mvp.model * vec4(inPosition, 1.0);
}