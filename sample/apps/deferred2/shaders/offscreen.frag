#version 450

layout(binding = 1) uniform sampler2D texColorMap;
layout(binding = 2) uniform sampler2D texNormalMap;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outAlbedo;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inTexCoord;

void main()
{
    outPosition = vec4(inPos, 1.0);

    // Calculate normal in tangent space
    vec3 N = normalize(inNormal);
    vec3 T = normalize(inTangent);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    vec3 tangentNormal = TBN * normalize(texture(texNormalMap, inTexCoord).xyz * 2.0 - vec3(1.0));
    outNormal = vec4(tangentNormal, 1.0);

    outAlbedo = texture(texColorMap, inTexCoord);
}