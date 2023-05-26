#pragma once

#include "vkt/gpu/buffer.h"
#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/queue.h"
#include "vkt/gpu/shader_module.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"
#include "vkt/gpu/texture_view.h"

#include "sample.h"

using namespace vkt;

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
};

class TriangleSample : public Sample
{
public:
    TriangleSample() = default;
    TriangleSample(int argc, char** argv);
    ~TriangleSample() override;

private:
    void createRenderPipeline();
    void createCommandBuffers();

    void draw() override;

private:
    // data
    std::vector<Vertex> m_vertices{};

    // wrapper
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;

    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;

    std::unique_ptr<Queue> m_renderQueue = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;

    std::unique_ptr<Buffer> m_buffer = nullptr;

    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    std::unique_ptr<ShaderModule> m_vertexShaderModule = nullptr;
    std::unique_ptr<ShaderModule> m_fragmentShaderModule = nullptr;

    std::vector<std::unique_ptr<CommandBuffer>> m_commandBuffers{};
};
