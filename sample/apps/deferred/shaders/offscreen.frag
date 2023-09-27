#version 450

layout(binding = 1) uniform sampler2D texColorMap;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outAlbedo;

layout(location = 0) in vec2 inTexCoord;

void main()
{
    outPosition = texture(texColorMap, inTexCoord);
    outNormal = vec4(1.0, 0.0, 0.0, 1.0);
    outAlbedo = vec4(0.0, 0.0, 1.0, 1.0);
}