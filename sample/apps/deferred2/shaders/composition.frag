#version 450

layout(location = 0) in vec2 inTexCoord;
layout(location = 0) out vec4 outColor;

// layout(binding = 0) uniform sampler2D texSamplerPosition;
// layout(binding = 1) uniform sampler2D texSamplerNormal;
// layout(binding = 2) uniform sampler2D texSamplerAlbedo;
layout(binding = 0) uniform sampler samplerPosition;
layout(binding = 1) uniform sampler samplerNormal;
layout(binding = 2) uniform sampler samplerAlbedo;
layout(binding = 3) uniform texture2D texPosition;
layout(binding = 4) uniform texture2D texNormal;
layout(binding = 5) uniform texture2D texAlbedo;

struct Light
{
    vec3 position;
    vec3 color;
};

#define maxLightCount 10000
#define ambient 0.0

// std140 for only uniform, consider alignment such as vec3
layout(std140, binding = 6) uniform UBO
{
    Light lights[maxLightCount];
    vec3 cameraPosition;
    int lightCount;
    int padding1;
    int padding2;
    int padding3;
}
ubo;

void applyLight(vec2 texCoord)
{
    // Get G-Buffer values
    // vec3 position = texture(texSamplerPosition, texCoord).rgb;
    // vec3 normal = texture(texSamplerNormal, texCoord).rgb;
    // vec4 albedo = texture(texSamplerAlbedo, texCoord);
    vec3 position = texture(sampler2D(texPosition, samplerPosition), texCoord).rgb;
    vec3 normal = texture(sampler2D(texNormal, samplerNormal), texCoord).rgb;
    vec4 albedo = texture(sampler2D(texAlbedo, samplerAlbedo), texCoord);

    // Ambient part
    vec3 color = albedo.rgb * ambient;

    for (int i = 0; i < ubo.lightCount; ++i)
    {

        // Vector to light
        vec3 L = ubo.lights[i].position.xyz - position;
        // Distance from light to fragment position
        float dist = length(L);

        // Viewer to fragment
        vec3 V = ubo.cameraPosition - position;
        V = normalize(V);

        float range = 30.0f;
        // if (dist < range)
        {
            // Light to fragment
            L = normalize(L);

            // Attenuation
            float atten = range / (pow(dist, 2.0) + 1.0);

            // Diffuse part
            vec3 N = normalize(normal);
            float NdotL = max(0.0, dot(N, L));
            vec3 diff = ubo.lights[i].color * albedo.rgb * NdotL * atten;

            // Specular part
            // Specular map values are stored in alpha of albedo mrt
            vec3 R = reflect(-L, N);
            float NdotR = max(0.0, dot(R, V));
            vec3 spec = ubo.lights[i].color * albedo.a * pow(NdotR, 16.0) * atten;

            color += diff + spec;
        }
    }

    outColor = vec4(color, 1.0f);
}

void main()
{
    if (inTexCoord.x < 0.5 && inTexCoord.y < 0.5)
    {
        applyLight(inTexCoord * 2.0f);
    }
    else if (inTexCoord.x < 0.5 && inTexCoord.y >= 0.5)
    {
        // outColor = texture(texSamplerNormal, vec2(inTexCoord.x * 2.0f, inTexCoord.y * 2.0f - 1.0f));
        outColor = texture(sampler2D(texNormal, samplerNormal), vec2(inTexCoord.x * 2.0f, inTexCoord.y * 2.0f - 1.0f));
    }
    else if (inTexCoord.x >= 0.5 && inTexCoord.y < 0.5)
    {
        // outColor = texture(texSamplerAlbedo, vec2(inTexCoord.x * 2.0f - 1.0f, inTexCoord.y * 2.0f));
        outColor = texture(sampler2D(texAlbedo, samplerNormal), vec2(inTexCoord.x * 2.0f - 1.0f, inTexCoord.y * 2.0f));
    }
    else
    {
        // outColor = texture(texSamplerPosition, vec2(inTexCoord.x * 2.0f - 1.0f, inTexCoord.y * 2.0f - 1.0f));
        outColor = texture(sampler2D(texPosition, samplerNormal), vec2(inTexCoord.x * 2.0f - 1.0f, inTexCoord.y * 2.0f - 1.0f));
    }
}