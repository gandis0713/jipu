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

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outTangent;
layout(location = 3) out vec2 outTexCoord;

void main()
{
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inPos, 1.0);

    // Position in world space
    outPos = vec3(mvp.model * vec4(inPos, 1.0));

    // Normal in world space
    mat3 modelWorldTranspos = transpose(inverse(mat3(mvp.model))); // refer to : https://webglfundamentals.org/webgl/lessons/ko/webgl-3d-lighting-directional.html
    outNormal = modelWorldTranspos * normalize(inNormal);
    outTangent = modelWorldTranspos * normalize(vec3(inTangent));

    outTexCoord = inTexCoord;
}