#include "camera.h"
#include "file.h"
#include "sample.h"

#include "jipu/buffer.h"
#include "jipu/command_buffer.h"
#include "jipu/command_encoder.h"
#include "jipu/device.h"
#include "jipu/instance.h"
#include "jipu/physical_device.h"
#include "jipu/pipeline.h"
#include "jipu/pipeline_layout.h"
#include "jipu/queue.h"
#include "jipu/surface.h"
#include "jipu/swapchain.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace jipu
{

class BlendSample : public Sample
{
public:
    BlendSample() = delete;
    BlendSample(const SampleDescriptor& descriptor);
    ~BlendSample() override;

    void init() override;
    void update() override;
    void draw() override;

private:
    void updateImGui();

private:
    void createCamera();

    void updateUniformBuffer();

private:
    void createCommandBuffer();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffer();
    void createBindingGroupLayout();
    void createBindingGroup();
    void createRenderPipeline();

private:
    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<Buffer> m_indexBuffer = nullptr;
    std::unique_ptr<Buffer> m_uniformBuffer = nullptr;
    std::unique_ptr<BindingGroupLayout> m_bindingGroupLayout = nullptr;
    std::unique_ptr<BindingGroup> m_bindingGroup = nullptr;
    std::unique_ptr<PipelineLayout> m_renderPipelineLayout = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

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

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
    };

    std::vector<uint16_t> m_indices{ 0, 1, 2 };
    std::vector<Vertex>
        m_vertices{
            { { -200, -500, 0.0 }, { 1.0, 0.0, 0.0 } },
            { { -700, 500, 0.0 }, { 1.0, 0.0, 0.0 } },
            { { 300, 500, 0.0 }, { 1.0, 0.0, 0.0 } },
        };

    uint32_t m_sampleCount = 1;
    std::unique_ptr<Camera> m_camera = nullptr;

    uint32_t m_blendColorSrcFactor = static_cast<uint32_t>(BlendFactor::kOne);
    uint32_t m_blendColorDstFactor = static_cast<uint32_t>(BlendFactor::kZero);
    uint32_t m_blendColorOp = static_cast<uint32_t>(BlendOperation::kAdd);
    uint32_t m_blendAlphaSrcFactor = static_cast<uint32_t>(BlendFactor::kOne);
    uint32_t m_blendAlphaDstFactor = static_cast<uint32_t>(BlendFactor::kZero);
    uint32_t m_blendAlphaOp = static_cast<uint32_t>(BlendOperation::kAdd);
};

} // namespace jipu