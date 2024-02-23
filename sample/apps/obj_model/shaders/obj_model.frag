#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(set = 1, binding = 0) uniform sampler spl;
layout(set = 1, binding = 1) uniform texture2D tex;

void main()
{
    outColor = texture(sampler2D(tex, spl), fragTexCoord);
}