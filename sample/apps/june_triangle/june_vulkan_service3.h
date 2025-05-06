#pragma once

#include "june/june.h"
#include "june/june_vulkan_service.h"

#include "camera.h"
#include "file.h"

namespace jipu
{

class JuneVulkanService3 : public JuneVulkanService
{
public:
    JuneVulkanService3(const JuneServiceDescriptor& descriptor);
    ~JuneVulkanService3();

    void begin() override;
    void work() override;

private:
    void createOffscreenTexture();
    void createOffscreenTextureView();
    void createOffscreenVertexBuffer();
    void createOffscreenIndexBuffer();
    void createOffscreenUniformBuffer();
    void createOffscreenBindGroupLayout();
    void createOffscreenBindGroup();
    void createOffscreenRenderPipeline();

    void createOnscreenTexture();
    void createOnscreenTextureView();
    void createOnscreenVertexBuffer();
    void createOnscreenIndexBuffer();
    void createOnscreenSampler();
    void createOnscreenBindGroupLayout();
    void createOnscreenBindGroup();
    void createOnscreenRenderPipeline();

    void createCamera();
    void updateOffscreenUniformBuffer();

private:
    struct
    {
        std::unique_ptr<Texture> renderTexture = nullptr;
        std::unique_ptr<TextureView> renderTextureView = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        std::unique_ptr<Buffer> indexBuffer = nullptr;
        std::unique_ptr<Buffer> uniformBuffer = nullptr;
        std::unique_ptr<BindGroupLayout> bindGroupLayout = nullptr;
        std::unique_ptr<BindGroup> bindGroup = nullptr;
        std::unique_ptr<PipelineLayout> renderPipelineLayout = nullptr;
        std::unique_ptr<RenderPipeline> renderPipeline = nullptr;
        VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
        VkImage image{ VK_NULL_HANDLE };
    } m_offscreen;

    struct
    {
        std::unique_ptr<Texture> renderTexture = nullptr;
        std::unique_ptr<TextureView> renderTextureView = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        std::unique_ptr<Buffer> indexBuffer = nullptr;
        std::unique_ptr<Sampler> sampler = nullptr;
        std::unique_ptr<BindGroupLayout> bindGroupLayout = nullptr;
        std::unique_ptr<BindGroup> bindGroup = nullptr;
        std::unique_ptr<PipelineLayout> renderPipelineLayout = nullptr;
        std::unique_ptr<RenderPipeline> renderPipeline = nullptr;
        VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
        VkImage image{ VK_NULL_HANDLE };
    } m_onscreen;

    struct MVP
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct UBO
    {
        MVP mvp;
    } m_ubo;

    struct OffscreenVertex
    {
        glm::vec3 pos;
        glm::vec3 color;
    };

    std::vector<OffscreenVertex> m_offscreenVertices{
        { { 0.0, -500, 0.0 }, { 1.0, 0.0, 0.0 } },
        { { -500, 500, 0.0 }, { 0.0, 1.0, 0.0 } },
        { { 500, 500, 0.0 }, { 0.0, 0.0, 1.0 } },
    };
    std::vector<uint16_t> m_offscreenIndices{ 0, 1, 2 };

    struct OnscreenVertex
    {
        glm::vec3 pos;
        glm::vec2 texCoord;
    };
    std::vector<OnscreenVertex> m_onscreenVertices{
        { { -1.0, -1.0, 0.0 }, { 0.0, 0.0 } },
        { { -1.0, 1.0, 0.0 }, { 0.0, 1.0 } },
        { { 1.0, 1.0, 0.0 }, { 1.0, 1.0 } },
        { { 1.0, -1.0, 0.0 }, { 1.0, 0.0 } },
    };
    std::vector<uint16_t> m_onscreenIndices{ 0, 1, 3, 1, 2, 3 };

    uint32_t m_sampleCount = 1; // use only 1, because there is not resolve texture.
    std::unique_ptr<Camera> m_camera = nullptr;
};

} // namespace jipu