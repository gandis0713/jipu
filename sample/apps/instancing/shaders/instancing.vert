#version 450

layout(location = 0) in vec3 inPosition;    // binding index = 0
layout(location = 1) in vec3 inColor;       // binding index = 0
layout(location = 2) in vec3 inTranslation; // binding index = 1

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
    vec4 position = ubo.orientation * vec4(inPosition, 1.0);

    gl_Position = ubo.mvp.proj * ubo.mvp.view * ubo.mvp.model * vec4(position.xyz + inTranslation, 1.0);
    outColor = inColor;
}