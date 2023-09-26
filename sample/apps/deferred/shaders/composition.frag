#version 450

layout(location = 0) in vec2 inTexCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler1;
layout(binding = 1) uniform sampler2D texSampler2;

void main()
{
    if (inTexCoord.y > 0.5)
    {
        outColor = texture(texSampler1, inTexCoord);
    }
    else
    {
        outColor = texture(texSampler2, inTexCoord);
    }
}