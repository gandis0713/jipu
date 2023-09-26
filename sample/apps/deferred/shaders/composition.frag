#version 450

layout(location = 0) in vec2 inTexCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSamplerPosition;
layout(binding = 1) uniform sampler2D texSamplerNormal;
layout(binding = 2) uniform sampler2D texSamplerAlbedo;

void main()
{
    if (inTexCoord.x < 0.5 && inTexCoord.y < 0.5)
    {
        outColor = texture(texSamplerPosition, inTexCoord * 2.0f);
    }
    else if (inTexCoord.x < 0.5 && inTexCoord.y >= 0.5)
    {
        outColor = texture(texSamplerNormal, vec2(inTexCoord.x * 2.0f, inTexCoord.y * 2.0f - 1.0f));
    }
    else if (inTexCoord.x >= 0.5 && inTexCoord.y < 0.5)
    {
        outColor = texture(texSamplerAlbedo, vec2(inTexCoord.x * 2.0f - 1.0f, inTexCoord.y * 2.0f));
    }
    else
    {
        discard;
    }
}