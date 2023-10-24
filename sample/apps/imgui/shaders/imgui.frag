#version 450

// layout (binding = 0) uniform sampler2D fontSampler;

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outColor;

void main()
{
    vec4 inColor = vec4(0.0, 0.0, 0.0, 1.0);
    // outColor = inColor * texture(fontSampler, inUV);
    outColor = inColor;
}