#version 450

layout(location = 0) in vec2 inTexCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler imageSampler;
layout(binding = 1) uniform texture2D imageTexture;

void main()
{
    outColor = texture(sampler2D(imageTexture, imageSampler), inTexCoord);
}