#pragma once

#include "june/june.h"
#include "june/june_vulkan_service.h"

#include "camera.h"
#include "file.h"

namespace jipu
{

class JuneVulkanService2 : public JuneVulkanService
{
public:
    JuneVulkanService2(const JuneServiceDescriptor& descriptor);
    ~JuneVulkanService2();

    void begin() override;
    void work() override;

    JuneServiceShareObjects getSharingObject() const override;
    void setSharedObjects(const JuneServiceShareObjects& sharedObjects) override;

private:
    void createOnscreenTexture();
    void createOnscreenTextureView();
    void createOnscreenVertexBuffer();
    void createOnscreenIndexBuffer();
    void createOnscreenSampler();
    void createOnscreenBindGroupLayout();
    void createOnscreenBindGroup();
    void createOnscreenRenderPipeline();

    bool m_isShared{ false };

private:
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
        VkImage image{ VK_NULL_HANDLE };
        VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
        JuneFence fence{ nullptr };
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