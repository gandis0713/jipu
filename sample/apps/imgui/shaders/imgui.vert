#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(push_constant) uniform PushConstants
{
    vec2 scale;
    vec2 translate;
}
pushConstants;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outUV = inUV;
    gl_Position = vec4(inPosition * pushConstants.scale + pushConstants.translate, 0.0, 1.0);
}