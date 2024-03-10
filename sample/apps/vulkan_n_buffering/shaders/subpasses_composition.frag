#version 450

layout(location = 0) in vec2 inTexCoord;
layout(location = 0) out vec4 outColor;

// layout(binding = 0) uniform sampler2D texSamplerPosition;
// layout(binding = 1) uniform sampler2D texSamplerNormal;
// layout(binding = 2) uniform sampler2D texSamplerAlbedo;

// layout(set = 1, binding = 0) uniform sampler samplerPosition;
// layout(set = 1, binding = 1) uniform sampler samplerNormal;
// layout(set = 1, binding = 2) uniform sampler samplerAlbedo;
// layout(set = 1, binding = 3) uniform texture2D texPosition;
// layout(set = 1, binding = 4) uniform texture2D texNormal;
// layout(set = 1, binding = 5) uniform texture2D texAlbedo;

layout(input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput inputPosition;
layout(input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputNormal;
layout(input_attachment_index = 2, set = 1, binding = 2) uniform subpassInput inputAlbedo;

struct Light
{
    vec3 position;
    vec3 color;
};

#define maxLightCount 1000
#define ambient 0.0

// std140 for only uniform, consider alignment such as vec3
layout(std140, set = 0, binding = 0) uniform UBO
{
    Light lights[maxLightCount];
    vec3 cameraPosition;
    int lightCount;
    int showTexture;
    int padding1;
    int padding2;
}
ubo;

void applyLight()
{
    // Get G-Buffer values
    // vec3 position = texture(texSamplerPosition, texCoord).rgb;
    // vec3 normal = texture(texSamplerNormal, texCoord).rgb;
    // vec4 albedo = texture(texSamplerAlbedo, texCoord);

    // vec3 position = texture(sampler2D(texPosition, samplerPosition), texCoord).rgb;
    // vec3 normal = texture(sampler2D(texNormal, samplerNormal), texCoord).rgb;
    // vec4 albedo = texture(sampler2D(texAlbedo, samplerAlbedo), texCoord);

    vec3 position = subpassLoad(inputPosition).rgb;
    vec3 normal = subpassLoad(inputNormal).rgb;
    vec4 albedo = subpassLoad(inputAlbedo);

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
    if (ubo.showTexture == 0)
    {
        applyLight();
    }
    else if (ubo.showTexture == 1)
    {
        outColor = subpassLoad(inputPosition);
    }
    else if (ubo.showTexture == 2)
    {
        outColor = subpassLoad(inputNormal);
    }
    else if (ubo.showTexture == 3)
    {
        outColor = subpassLoad(inputAlbedo);
    }
}