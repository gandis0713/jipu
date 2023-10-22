

#include "file.h"
#include "sample.h"

#include "vkt/gpu/buffer.h"
#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/command_encoder.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/pipeline_layout.h"
#include "vkt/gpu/queue.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#if defined(__ANDROID__) || defined(ANDROID)

    // GameActivity's C/C++ code
    #include <game-activity/GameActivity.cpp>
    #include <game-text-input/gametextinput.cpp>

// // Glue from GameActivity to android_main()
// // Passing GameActivity event from main thread to app native thread.
extern "C"
{
    #include <game-activity/native_app_glue/android_native_app_glue.c>
}

#endif

namespace vkt
{

class TriangleSample : public Sample
{
public:
    TriangleSample() = delete;
    TriangleSample(const SampleDescriptor& descriptor);
    ~TriangleSample() override;

    void init() override;
    void draw() override;

private:
    void createDevier();
    void createPhysicalDevice();
    void createSurface();
    void createDevice();
    void createSwapchain();
    void createCommandBuffer();
    void createQueue();
    void createVertexBuffer();
    void createRenderPipeline();

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;
    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;

    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
    };

    std::vector<Vertex> m_vertices{
        { { 0.0, -0.5, 0.0 }, { 1.0, 0.0, 0.0 } },
        { { -0.5, 0.5, 0.0 }, { 0.0, 1.0, 0.0 } },
        { { 0.5, 0.5, 0.0 }, { 0.0, 0.0, 1.0 } },
    };

    uint32_t m_sampleCount = 1;
};

TriangleSample::TriangleSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

TriangleSample::~TriangleSample()
{
    m_renderPipeline.reset();
    m_vertexBuffer.reset();
    m_queue.reset();
    m_commandBuffer.reset();
    m_swapchain.reset();
    m_device.reset();
    m_surface.reset();
    m_physicalDevice.reset();
    m_driver.reset();
}

void TriangleSample::init()
{
    createDevier();
    createPhysicalDevice();
    createSurface();
    createDevice();
    createSwapchain();
    createCommandBuffer();
    createQueue();
    createVertexBuffer();
    createRenderPipeline();
}

void TriangleSample::draw()
{
    auto swapchainIndex = m_swapchain->acquireNextTexture();

    {
        ColorAttachment attachment{};
        attachment.clearValue = { .float32 = { 0.0, 0.0, 0.0, 0.0 } };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;
        attachment.renderView = m_swapchain->getTextureView(swapchainIndex);
        attachment.resolveView = nullptr;

        RenderPassEncoderDescriptor renderPassDescriptor;
        renderPassDescriptor.sampleCount = m_sampleCount;
        renderPassDescriptor.colorAttachments = { attachment };

        CommandEncoderDescriptor commandDescriptor{};
        auto commadEncoder = m_commandBuffer->createCommandEncoder(commandDescriptor);

        auto renderPassEncoder = commadEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(m_renderPipeline.get());
        renderPassEncoder->setVertexBuffer(m_vertexBuffer.get());
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->draw(static_cast<uint32_t>(m_vertices.size()));
        renderPassEncoder->end();

        m_queue->submit({ commadEncoder->finish() }, m_swapchain.get());
    }
}

void TriangleSample::createDevier()
{
    DriverDescriptor descriptor{};
    descriptor.type = DriverType::VULKAN;

    m_driver = Driver::create(descriptor);
}

void TriangleSample::createPhysicalDevice()
{
    PhysicalDeviceDescriptor descriptor{};
    descriptor.index = 0; // TODO: select device.

    m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
}

void TriangleSample::createDevice()
{
    SurfaceDescriptor descriptor{};
    descriptor.windowHandle = getWindowHandle();

    m_surface = m_driver->createSurface(descriptor);
}

void TriangleSample::createSurface()
{
    DeviceDescriptor descriptor{};

    m_device = m_physicalDevice->createDevice(descriptor);
}

void TriangleSample::createSwapchain()
{
#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat textureFormat = TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
#else
    TextureFormat textureFormat = TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
#endif

    SwapchainDescriptor descriptor{};
    descriptor.width = m_width;
    descriptor.height = m_height;
    descriptor.surface = m_surface.get();
    descriptor.textureFormat = textureFormat;
    descriptor.colorSpace = ColorSpace::kSRGBNonLinear;
    descriptor.presentMode = PresentMode::kFifo;

    m_swapchain = m_device->createSwapchain(descriptor);
}

void TriangleSample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    descriptor.usage = CommandBufferUsage::kOneTime;

    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void TriangleSample::createQueue()
{
    QueueDescriptor descriptor{};
    descriptor.flags = QueueFlagBits::kGraphics;

    m_queue = m_device->createQueue(descriptor);
}

void TriangleSample::createVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_vertices.size() * sizeof(Vertex);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_vertexBuffer->map();
    memcpy(pointer, m_vertices.data(), descriptor.size);
    m_vertexBuffer->unmap();
}

void TriangleSample::createRenderPipeline()
{
    // render pipeline layout
    std::unique_ptr<PipelineLayout> renderPipelineLayout = nullptr;
    {
        PipelineLayoutDescriptor descriptor{};

        renderPipelineLayout = m_device->createPipelineLayout(descriptor);
    }

    // input assembly stage
    InputAssemblyStage inputAssemblyStage{};
    {
        inputAssemblyStage.topology = PrimitiveTopology::kTriangleList;
    }

    // vertex shader module
    std::unique_ptr<ShaderModule> vertexShaderModule = nullptr;
    {
        ShaderModuleDescriptor descriptor{};
        std::vector<char> vertexShaderSource = utils::readFile(m_appDir / "triangle.vert.spv", m_handle);
        descriptor.code = vertexShaderSource.data();
        descriptor.codeSize = static_cast<uint32_t>(vertexShaderSource.size());

        vertexShaderModule = m_device->createShaderModule(descriptor);
    }

    // vertex stage
    VertexStage vertexStage{};
    {
        VertexAttribute positionAttribute{};
        positionAttribute.format = VertexFormat::kSFLOATx3;
        positionAttribute.offset = offsetof(Vertex, pos);

        VertexAttribute colorAttribute{};
        colorAttribute.format = VertexFormat::kSFLOATx3;
        colorAttribute.offset = offsetof(Vertex, color);

        VertexInputLayout vertexInputLayout{};
        vertexInputLayout.mode = VertexMode::kVertex;
        vertexInputLayout.stride = sizeof(Vertex);
        vertexInputLayout.attributes = { positionAttribute, colorAttribute };

        vertexStage.entryPoint = "main";
        vertexStage.shaderModule = vertexShaderModule.get();
        vertexStage.layouts = { vertexInputLayout };
    }

    // rasterization
    RasterizationStage rasterizationStage{};
    {
        rasterizationStage.cullMode = CullMode::kNone;
        rasterizationStage.frontFace = FrontFace::kCounterClockwise;
        rasterizationStage.sampleCount = m_sampleCount;
    }

    // fragment shader module
    std::unique_ptr<ShaderModule> fragmentShaderModule = nullptr;
    {
        ShaderModuleDescriptor descriptor{};
        std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "triangle.frag.spv", m_handle);
        descriptor.code = fragmentShaderSource.data();
        descriptor.codeSize = static_cast<uint32_t>(fragmentShaderSource.size());

        fragmentShaderModule = m_device->createShaderModule(descriptor);
    }

    // fragment
    FragmentStage fragmentStage{};
    {
        FragmentStage::Target target{};
        target.format = m_swapchain->getTextureFormat();

        fragmentStage.targets = { target };
        fragmentStage.entryPoint = "main";
        fragmentStage.shaderModule = fragmentShaderModule.get();
    }

    // depth/stencil

    // render pipeline
    RenderPipelineDescriptor descriptor{};
    descriptor.inputAssembly = inputAssemblyStage;
    descriptor.vertex = vertexStage;
    descriptor.rasterization = rasterizationStage;
    descriptor.fragment = fragmentStage;
    descriptor.layout = renderPipelineLayout.get();

    m_renderPipeline = m_device->createRenderPipeline(descriptor);
}

} // namespace vkt

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Triangle", app },
        ""
    };

    vkt::TriangleSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Triangle", nullptr },
        argv[0]
    };

    vkt::TriangleSample sample(descriptor);

    return sample.exec();
}

#endif
