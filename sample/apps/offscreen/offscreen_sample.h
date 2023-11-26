

#include "camera.h"
#include "file.h"
#include "im_gui.h"
#include "sample.h"

#include "jipu/buffer.h"
#include "jipu/command_buffer.h"
#include "jipu/command_encoder.h"
#include "jipu/device.h"
#include "jipu/driver.h"
#include "jipu/physical_device.h"
#include "jipu/pipeline.h"
#include "jipu/pipeline_layout.h"
#include "jipu/queue.h"
#include "jipu/surface.h"
#include "jipu/swapchain.h"

namespace jipu
{

class OffscreenSample : public Sample, public Im_Gui
{
public:
    OffscreenSample() = delete;
    OffscreenSample(const SampleDescriptor& descriptor);
    ~OffscreenSample() override;

    void init() override;
    void update() override;
    void draw() override;

private:
    void updateImGui() override;

private:
    void updateOffscreenUniformBuffer();

private:
    void createDevier();
    void createPhysicalDevice();
    void createSurface();
    void createDevice();
    void createSwapchain();
    void createCommandBuffer();
    void createQueue();

    void createOffscreenTexture();
    void createOffscreenTextureView();
    void createOffscreenVertexBuffer();
    void createOffscreenIndexBuffer();
    void createOffscreenUniformBuffer();
    void createOffscreenBindingGroupLayout();
    void createOffscreenBindingGroup();
    void createOffscreenRenderPipeline();

    void createOnscreenSwapchain();
    void createOnscreenVertexBuffer();
    void createOnscreenIndexBuffer();
    void createOnscreenSampler();
    void createOnscreenBindingGroupLayout();
    void createOnscreenBindingGroup();
    void createOnscreenRenderPipeline();

    void createCamera();

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;

    struct
    {
        std::unique_ptr<Texture> renderTexture = nullptr;
        std::unique_ptr<TextureView> renderTextureView = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        std::unique_ptr<Buffer> indexBuffer = nullptr;
        std::unique_ptr<Buffer> uniformBuffer = nullptr;
        std::unique_ptr<BindingGroupLayout> bindingGroupLayout = nullptr;
        std::unique_ptr<BindingGroup> bindingGroup = nullptr;
        std::unique_ptr<PipelineLayout> renderPipelineLayout = nullptr;
        std::unique_ptr<RenderPipeline> renderPipeline = nullptr;
    } m_offscreen;

    struct
    {
        std::unique_ptr<Swapchain> swapchain = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        std::unique_ptr<Buffer> indexBuffer = nullptr;
        std::unique_ptr<Sampler> sampler = nullptr;
        std::unique_ptr<BindingGroupLayout> bindingGroupLayout = nullptr;
        std::unique_ptr<BindingGroup> bindingGroup = nullptr;
        std::unique_ptr<PipelineLayout> renderPipelineLayout = nullptr;
        std::unique_ptr<RenderPipeline> renderPipeline = nullptr;
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
    };

    std::vector<OnscreenVertex> m_onscreenVertices{
        { { -1.0, -1.0, 0.0 } },
        { { -1.0, 1.0, 0.0 } },
        { { 1.0, 1.0, 0.0 } },
        { { 1.0, -1.0, 0.0 } },
    };
    std::vector<uint16_t> m_onscreenIndices{ 0, 1, 3, 1, 2, 3 };

    uint32_t m_sampleCount = 1;
    std::unique_ptr<Camera> m_camera = nullptr;
};
} // namespace jipu