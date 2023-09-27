#version 450

layout(binding = 1) uniform sampler2D texColorMap;
layout(binding = 2) uniform sampler2D texNormalMap;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outAlbedo;

layout(location = 0) in vec2 inTexCoord;

void main()
{
    outPosition = vec4(1.0, 1.0, 0.0, 1.0);
    outNormal = texture(texNormalMap, inTexCoord);
    outAlbedo = texture(texColorMap, inTexCoord);
}