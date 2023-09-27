#version 450

layout(binding = 0) uniform MVP
{
    mat4 model;
    mat4 view;
    mat4 proj;
}
mvp;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

void main()
{
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inPos, 1.0);
    outTexCoord = inTexCoord;
}