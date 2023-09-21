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
#include "vkt/gpu/render_pass_encoder.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

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

class DeferredSample : public Sample
{
public:
    DeferredSample() = delete;
    DeferredSample(const SampleDescriptor& descriptor);
    ~DeferredSample() override;

public:
    void init() override;
    void draw() override;

private:
    void createDriver();
    void createPhysicalDevice();
    void createSurface();
    void createDevice();
    void createSwapchain();

    void createOffscreenDepthStencilTexture();
    void createOffscreenDepthStencilTextureView();
    void createOffscreenPipelineLayout();
    void createOffscreenPipeline();
    void createOffscreenVertexBuffer();

    void createCompositionDepthStencilTexture();
    void createCompositionDepthStencilTextureView();
    void createCompositionPipelineLayout();
    void createCompositionPipeline();
    void createCompositionVertexBuffer();

    void createCommandBuffer();
    void createQueue();

private:
    struct OffscreenVertex
    {
        glm::vec3 position;
    };

    struct CompositionVertex
    {
        glm::vec3 position;
        glm::vec2 textureCoordinate;
    };

    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;

    struct
    {
        std::unique_ptr<Texture> depthStencilTexture = nullptr;
        std::unique_ptr<TextureView> depthStencilTextureView = nullptr;
        std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
        std::unique_ptr<Pipeline> pipeline = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
    } m_offscreen;

    struct
    {
        std::unique_ptr<Texture> depthStencilTexture = nullptr;
        std::unique_ptr<TextureView> depthStencilTextureView = nullptr;
        std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
        std::unique_ptr<Pipeline> pipeline = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
    } m_composition;

    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;

    std::vector<OffscreenVertex> vertices = {
        { { 0.0, -0.5, 0.0 } },
        { { -0.5, 0.5, 0.0 } },
        { { 0.5, 0.5, 0.0 } }
    };

    std::vector<CompositionVertex> quadrangle = {
        { { -1.0, -1.0, 0.0 }, { 0.0, 0.0 } },
        { { -1.0, 1.0, 0.0 }, { 0.0, 1.0 } },
        { { 1.0, -1.0, 0.0 }, { 1.0, 0.0 } },
        { { 1.0, -1.0, 0.0 }, { 1.0, 0.0 } },
        { { -1.0, 1.0, 0.0 }, { 0.0, 1.0 } },
        { { 1.0, 1.0, 0.0 }, { 1.0, 1.0 } },
    };

    std::vector<glm::vec2> textureCoordinate = {

    };

    uint32_t m_sampleCount = 1;
};

DeferredSample::DeferredSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
    // Do not call init function.
}

DeferredSample::~DeferredSample()
{
    m_queue.reset();
    m_commandBuffer.reset();

    m_offscreen.vertexBuffer.reset();
    m_offscreen.pipeline.reset();
    m_offscreen.pipelineLayout.reset();
    m_offscreen.depthStencilTextureView.reset();
    m_offscreen.depthStencilTexture.reset();

    m_swapchain.reset();
    m_device.reset();

    m_surface.reset();
    m_physicalDevice.reset();
    m_driver.reset();
}

void DeferredSample::init()
{
    createDriver();
    createPhysicalDevice();
    createSurface();
    createDevice();
    createSwapchain();

    createOffscreenDepthStencilTexture();
    createOffscreenDepthStencilTextureView();
    createOffscreenPipelineLayout();
    createOffscreenPipeline();
    createOffscreenVertexBuffer();

    createCompositionDepthStencilTexture();
    createCompositionDepthStencilTextureView();
    createCompositionPipelineLayout();
    createCompositionPipeline();
    createCompositionVertexBuffer();

    createCommandBuffer();
    createQueue();
}

void DeferredSample::draw()
{
    int targetIndex = m_swapchain->acquireNextTexture();

    ColorAttachment colorAttachment{};
    colorAttachment.loadOp = LoadOp::kClear;
    colorAttachment.storeOp = StoreOp::kStore;
    colorAttachment.renderView = m_swapchain->getTextureViews()[targetIndex];
    colorAttachment.resolveView = nullptr;
    colorAttachment.clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };

    DepthStencilAttachment depthStencilAttachment{};
    depthStencilAttachment.textureView = m_offscreen.depthStencilTextureView.get();
    depthStencilAttachment.depthLoadOp = LoadOp::kClear;
    depthStencilAttachment.depthStoreOp = StoreOp::kStore;
    depthStencilAttachment.clearValue = { .depth = 1.0f, .stencil = 0 };

    RenderPassEncoderDescriptor renderPassEncoderDescriptor{};
    renderPassEncoderDescriptor.colorAttachments = { colorAttachment };
    renderPassEncoderDescriptor.depthStencilAttachment = depthStencilAttachment;

    CommandEncoderDescriptor commandEncoderDescriptor{};
    auto commandEncoder = m_commandBuffer->createCommandEncoder(commandEncoderDescriptor);

    auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassEncoderDescriptor);
    renderPassEncoder->setPipeline(m_offscreen.pipeline.get());
    renderPassEncoder->setVertexBuffer(m_offscreen.vertexBuffer.get());
    renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
    renderPassEncoder->setScissor(0, 0, m_width, m_height);
    renderPassEncoder->draw(static_cast<uint32_t>(vertices.size()));
    renderPassEncoder->end();

    m_queue->submit({ commandEncoder->finish() }, m_swapchain.get());
}

void DeferredSample::createDriver()
{
    DriverDescriptor descriptor;
    descriptor.type = DriverType::VULKAN;
    m_driver = Driver::create(descriptor);
}

void DeferredSample::createPhysicalDevice()
{
    PhysicalDeviceDescriptor descriptor;
    descriptor.index = 0; // TODO: find index from driver.
    m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
}

void DeferredSample::createSurface()
{
    SurfaceDescriptor descriptor;
    descriptor.windowHandle = getWindowHandle();
    m_surface = m_driver->createSurface(descriptor);
}

void DeferredSample::createSwapchain()
{
    if (m_surface == nullptr)
        throw std::runtime_error("Surface is null pointer.");

#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat textureFormat = TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
#else
    TextureFormat textureFormat = TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
#endif

    SwapchainDescriptor descriptor;
    descriptor.width = m_width;
    descriptor.height = m_height;
    descriptor.surface = m_surface.get();
    descriptor.colorSpace = ColorSpace::kSRGBNonLinear;
    descriptor.textureFormat = textureFormat;
    descriptor.presentMode = PresentMode::kFifo;

    m_swapchain = m_device->createSwapchain(descriptor);
}

void DeferredSample::createDevice()
{
    DeviceDescriptor descriptor;
    m_device = m_physicalDevice->createDevice(descriptor);
}

void DeferredSample::createOffscreenDepthStencilTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kD_32_SFloat;
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.usages = TextureUsageFlagBits::kDepthStencil;

    m_offscreen.depthStencilTexture = m_device->createTexture(descriptor);
}

void DeferredSample::createOffscreenDepthStencilTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kDepth;

    m_offscreen.depthStencilTextureView = m_offscreen.depthStencilTexture->createTextureView(descriptor);
}

void DeferredSample::createOffscreenPipelineLayout()
{
    PipelineLayoutDescriptor descriptor{};
    m_offscreen.pipelineLayout = m_device->createPipelineLayout(descriptor);
}

void DeferredSample::createOffscreenPipeline()
{
    // Input Assembly
    InputAssemblyStage inputAssembly{};
    inputAssembly.topology = PrimitiveTopology::kTriangleList;

    // shader module
    std::unique_ptr<ShaderModule> vertexShaderModule = nullptr;
    {
        std::vector<char> vertexSource = utils::readFile(m_appDir / "offscreen.vert.spv", m_handle);

        ShaderModuleDescriptor shaderModuleDescriptor;
        shaderModuleDescriptor.code = vertexSource.data();
        shaderModuleDescriptor.codeSize = vertexSource.size();

        vertexShaderModule = m_device->createShaderModule(shaderModuleDescriptor);
    }
    // Vertex Shader
    VertexStage vertexShage{};
    {
        // entry point
        vertexShage.entryPoint = "main";

        // input layout
        VertexInputLayout inputLayout;
        inputLayout.mode = VertexMode::kVertex;
        inputLayout.stride = sizeof(OffscreenVertex);

        VertexAttribute attribute;
        attribute.format = VertexFormat::kSFLOATx3;
        attribute.offset = offsetof(OffscreenVertex, position);

        inputLayout.attributes = { attribute };

        vertexShage.layouts = { inputLayout };

        vertexShage.shaderModule = vertexShaderModule.get();
    }

    // Rasterization
    RasterizationStage rasterizationStage{};
    rasterizationStage.sampleCount = m_sampleCount;
    rasterizationStage.cullMode = CullMode::kBack;
    rasterizationStage.frontFace = FrontFace::kCounterClockwise;

    // shader module
    std::unique_ptr<ShaderModule> fragmentShaderModule = nullptr;
    {
        std::vector<char> fragmentSource = utils::readFile(m_appDir / "offscreen.frag.spv", m_handle);

        ShaderModuleDescriptor shaderModuleDescriptor;
        shaderModuleDescriptor.code = fragmentSource.data();
        shaderModuleDescriptor.codeSize = fragmentSource.size();

        fragmentShaderModule = m_device->createShaderModule(shaderModuleDescriptor);
    }

    // Fragment Shader
    FragmentStage fragmentStage{};
    {
        // entry point
        fragmentStage.entryPoint = "main";

        // targets
        FragmentStage::Target target{};
        target.format = m_swapchain->getTextureFormat();
        fragmentStage.targets = { target };
        fragmentStage.shaderModule = fragmentShaderModule.get();
    }

    DepthStencilStage depthStencil{};
    depthStencil.format = m_offscreen.depthStencilTexture->getFormat();

    RenderPipelineDescriptor renderPipelineDescriptor;
    renderPipelineDescriptor.inputAssembly = inputAssembly;
    renderPipelineDescriptor.vertex = vertexShage;
    renderPipelineDescriptor.rasterization = rasterizationStage;
    renderPipelineDescriptor.fragment = fragmentStage;
    renderPipelineDescriptor.depthStencil = depthStencil;
    renderPipelineDescriptor.layout = m_offscreen.pipelineLayout.get();

    m_offscreen.pipeline = m_device->createRenderPipeline(renderPipelineDescriptor);
}

void DeferredSample::createOffscreenVertexBuffer()
{
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = sizeof(OffscreenVertex) * vertices.size();
    bufferDescriptor.usage = BufferUsageFlagBits::kVertex;

    m_offscreen.vertexBuffer = m_device->createBuffer(bufferDescriptor);
    void* mappedPointer = m_offscreen.vertexBuffer->map();
    memcpy(mappedPointer, vertices.data(), sizeof(OffscreenVertex) * vertices.size());
    m_offscreen.vertexBuffer->unmap();
}

void DeferredSample::createCompositionDepthStencilTexture()
{
}

void DeferredSample::createCompositionDepthStencilTextureView()
{
}

void DeferredSample::createCompositionPipelineLayout()
{
}

void DeferredSample::createCompositionPipeline()
{
}

void DeferredSample::createCompositionVertexBuffer()
{
}

void DeferredSample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    descriptor.usage = CommandBufferUsage::kOneTime;

    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void DeferredSample::createQueue()
{
    QueueDescriptor descriptor{};
    descriptor.flags = QueueFlagBits::kGraphics;

    m_queue = m_device->createQueue(descriptor);
}

} // namespace vkt

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Deferred Sample", app },
        ""
    };

    vkt::DeferredSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Deferred Sample", nullptr },
        argv[0]
    };

    vkt::DeferredSample sample(descriptor);

    return sample.exec();
}

#endif