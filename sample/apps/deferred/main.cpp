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

    void createOffscreenPositionColorAttachmentTexture();
    void createOffscreenPositionColorAttachmentTextureView();
    void createOffscreenNormalColorAttachmentTexture();
    void createOffscreenNormalColorAttachmentTextureView();
    void createOffscreenDepthStencilTexture();
    void createOffscreenDepthStencilTextureView();
    void createOffscreenPipelineLayout();
    void createOffscreenPipeline();
    void createOffscreenVertexBuffer();

    void createCompositionDepthStencilTexture();
    void createCompositionDepthStencilTextureView();
    void createCompositionBindingGroupLayout();
    void createCompositionBindingGroup();
    void createCompositionPipelineLayout();
    void createCompositionPipeline();
    void createCompositionVertexBuffer();

    void createCommandBuffer();
    void createQueue();

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;

    struct OffscreenVertex
    {
        glm::vec3 position;
    };
    struct
    {
        std::unique_ptr<Texture> positionColorAttachmentTexture = nullptr;
        std::unique_ptr<TextureView> positionColorAttachmentTextureView = nullptr;
        std::unique_ptr<Texture> normalColorAttachmentTexture = nullptr;
        std::unique_ptr<TextureView> normalColorAttachmentTextureView = nullptr;
        std::unique_ptr<Texture> depthStencilTexture = nullptr;
        std::unique_ptr<TextureView> depthStencilTextureView = nullptr;
        std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
        std::unique_ptr<Pipeline> pipeline = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        std::vector<OffscreenVertex> vertices{
            { { 0.0, -0.5, 0.0 } },
            { { -0.5, 0.5, 0.0 } },
            { { 0.5, 0.5, 0.0 } }
        };
    } m_offscreen;

    struct CompositionVertex
    {
        glm::vec3 position;
        glm::vec2 textureCoordinate;
    };
    struct
    {
        std::unique_ptr<Texture> depthStencilTexture = nullptr;
        std::unique_ptr<TextureView> depthStencilTextureView = nullptr;
        std::unique_ptr<BindingGroupLayout> bindingGroupLayout = nullptr;
        std::unique_ptr<BindingGroup> bindingGroup = nullptr;
        std::unique_ptr<Sampler> positionSampler = nullptr;
        std::unique_ptr<Sampler> normalSampler = nullptr;
        std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
        std::unique_ptr<Pipeline> pipeline = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        std::vector<CompositionVertex> vertices{
            { { -1.0, -1.0, 0.0 }, { 0.0, 0.0 } },
            { { -1.0, 1.0, 0.0 }, { 0.0, 1.0 } },
            { { 1.0, -1.0, 0.0 }, { 1.0, 0.0 } },
            { { 1.0, -1.0, 0.0 }, { 1.0, 0.0 } },
            { { -1.0, 1.0, 0.0 }, { 0.0, 1.0 } },
            { { 1.0, 1.0, 0.0 }, { 1.0, 1.0 } },
        };
    } m_composition;

    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;

    uint32_t m_sampleCount = 1;
};

DeferredSample::DeferredSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
    // Do not call init function.
}

DeferredSample::~DeferredSample()
{
    m_composition.vertexBuffer.reset();
    m_composition.pipeline.reset();
    m_composition.pipelineLayout.reset();
    m_composition.bindingGroupLayout.reset();
    m_composition.bindingGroup.reset();
    m_composition.positionSampler.reset();
    m_composition.normalSampler.reset();
    m_composition.depthStencilTextureView.reset();
    m_composition.depthStencilTexture.reset();

    m_offscreen.vertexBuffer.reset();
    m_offscreen.pipeline.reset();
    m_offscreen.pipelineLayout.reset();
    m_offscreen.depthStencilTextureView.reset();
    m_offscreen.depthStencilTexture.reset();
    m_offscreen.normalColorAttachmentTextureView.reset();
    m_offscreen.normalColorAttachmentTexture.reset();
    m_offscreen.positionColorAttachmentTextureView.reset();
    m_offscreen.positionColorAttachmentTexture.reset();

    m_queue.reset();
    m_commandBuffer.reset();

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

    createCommandBuffer();
    createQueue();

    createOffscreenPositionColorAttachmentTexture();
    createOffscreenPositionColorAttachmentTextureView();
    createOffscreenNormalColorAttachmentTexture();
    createOffscreenNormalColorAttachmentTextureView();
    createOffscreenDepthStencilTexture();
    createOffscreenDepthStencilTextureView();
    createOffscreenPipelineLayout();
    createOffscreenPipeline();
    createOffscreenVertexBuffer();

    createCompositionDepthStencilTexture();
    createCompositionDepthStencilTextureView();
    createCompositionBindingGroupLayout();
    createCompositionBindingGroup();
    createCompositionPipelineLayout();
    createCompositionPipeline();
    createCompositionVertexBuffer();
}

void DeferredSample::draw()
{

    CommandEncoderDescriptor commandEncoderDescriptor{};
    auto commandEncoder = m_commandBuffer->createCommandEncoder(commandEncoderDescriptor);

    // first pass
    {
        ColorAttachment positionColorAttachment{};
        positionColorAttachment.loadOp = LoadOp::kClear;
        positionColorAttachment.storeOp = StoreOp::kStore;
        positionColorAttachment.renderView = m_offscreen.positionColorAttachmentTextureView.get();
        positionColorAttachment.resolveView = nullptr;
        positionColorAttachment.clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };

        ColorAttachment normalColorAttachment{};
        normalColorAttachment.loadOp = LoadOp::kClear;
        normalColorAttachment.storeOp = StoreOp::kStore;
        normalColorAttachment.renderView = m_offscreen.normalColorAttachmentTextureView.get();
        normalColorAttachment.resolveView = nullptr;
        normalColorAttachment.clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };

        DepthStencilAttachment depthStencilAttachment{};
        depthStencilAttachment.textureView = m_offscreen.depthStencilTextureView.get();
        depthStencilAttachment.depthLoadOp = LoadOp::kClear;
        depthStencilAttachment.depthStoreOp = StoreOp::kStore;
        depthStencilAttachment.clearValue = { .depth = 1.0f, .stencil = 0 };

        RenderPassEncoderDescriptor renderPassEncoderDescriptor{};
        renderPassEncoderDescriptor.colorAttachments = { positionColorAttachment, normalColorAttachment };
        renderPassEncoderDescriptor.depthStencilAttachment = depthStencilAttachment;
        renderPassEncoderDescriptor.sampleCount = m_sampleCount;

        auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassEncoderDescriptor);
        renderPassEncoder->setPipeline(m_offscreen.pipeline.get());
        renderPassEncoder->setVertexBuffer(m_offscreen.vertexBuffer.get());
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->draw(static_cast<uint32_t>(m_offscreen.vertices.size()));
        renderPassEncoder->end();
    }
    // second pass
    {
        int targetIndex = m_swapchain->acquireNextTexture();

        ColorAttachment colorAttachment{};
        colorAttachment.loadOp = LoadOp::kClear;
        colorAttachment.storeOp = StoreOp::kStore;
        colorAttachment.renderView = m_swapchain->getTextureViews()[targetIndex];
        colorAttachment.resolveView = nullptr;
        colorAttachment.clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };

        DepthStencilAttachment depthStencilAttachment{};
        depthStencilAttachment.textureView = m_composition.depthStencilTextureView.get();
        depthStencilAttachment.depthLoadOp = LoadOp::kClear;
        depthStencilAttachment.depthStoreOp = StoreOp::kStore;
        depthStencilAttachment.clearValue = { .depth = 1.0f, .stencil = 0 };

        RenderPassEncoderDescriptor renderPassEncoderDescriptor{};
        renderPassEncoderDescriptor.colorAttachments = { colorAttachment };
        renderPassEncoderDescriptor.depthStencilAttachment = depthStencilAttachment;
        renderPassEncoderDescriptor.sampleCount = m_sampleCount;

        auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassEncoderDescriptor);
        renderPassEncoder->setPipeline(m_composition.pipeline.get());
        renderPassEncoder->setVertexBuffer(m_composition.vertexBuffer.get());
        renderPassEncoder->setBindingGroup(0, m_composition.bindingGroup.get());
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->draw(static_cast<uint32_t>(m_composition.vertices.size()));
        renderPassEncoder->end();
    }

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

void DeferredSample::createOffscreenPositionColorAttachmentTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = m_swapchain->getTextureFormat();
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.usage = TextureUsageFlagBits::kColorAttachment | TextureUsageFlagBits::kTextureBinding;

    m_offscreen.positionColorAttachmentTexture = m_device->createTexture(descriptor);
}

void DeferredSample::createOffscreenPositionColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.positionColorAttachmentTextureView = m_offscreen.positionColorAttachmentTexture->createTextureView(descriptor);
}

void DeferredSample::createOffscreenNormalColorAttachmentTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = m_swapchain->getTextureFormat();
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.usage = TextureUsageFlagBits::kColorAttachment | TextureUsageFlagBits::kTextureBinding;

    m_offscreen.normalColorAttachmentTexture = m_device->createTexture(descriptor);
}

void DeferredSample::createOffscreenNormalColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.normalColorAttachmentTextureView = m_offscreen.normalColorAttachmentTexture->createTextureView(descriptor);
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
    descriptor.usage = TextureUsageFlagBits::kDepthStencil;

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
        FragmentStage::Target positionTarget{};
        positionTarget.format = m_swapchain->getTextureFormat();

        FragmentStage::Target normalTarget{};
        normalTarget.format = m_swapchain->getTextureFormat();

        fragmentStage.targets = { positionTarget, normalTarget };
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
    bufferDescriptor.size = sizeof(OffscreenVertex) * m_offscreen.vertices.size();
    bufferDescriptor.usage = BufferUsageFlagBits::kVertex;

    m_offscreen.vertexBuffer = m_device->createBuffer(bufferDescriptor);
    void* mappedPointer = m_offscreen.vertexBuffer->map();
    memcpy(mappedPointer, m_offscreen.vertices.data(), bufferDescriptor.size);
    m_offscreen.vertexBuffer->unmap();
}

void DeferredSample::createCompositionDepthStencilTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kD_32_SFloat;
    descriptor.usage = TextureUsageFlagBits::kDepthStencil;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;

    m_composition.depthStencilTexture = m_device->createTexture(descriptor);
}

void DeferredSample::createCompositionDepthStencilTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kDepth;

    m_composition.depthStencilTextureView = m_offscreen.depthStencilTexture->createTextureView(descriptor);
}

void DeferredSample::createCompositionBindingGroupLayout()
{
    BindingGroupLayoutDescriptor descriptor{};

    SamplerBindingLayout positionSamplerBindingLayout{};
    positionSamplerBindingLayout.index = 0;
    positionSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

    SamplerBindingLayout normalSamplerBindingLayout{};
    normalSamplerBindingLayout.index = 1;
    normalSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

    descriptor.samplers = { positionSamplerBindingLayout, normalSamplerBindingLayout };

    m_composition.bindingGroupLayout = m_device->createBindingGroupLayout(descriptor);
}

void DeferredSample::createCompositionBindingGroup()
{
    SamplerBinding positionSamplerBinding{};
    {
        SamplerDescriptor samplerDescriptor{};
        samplerDescriptor.addressModeU = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeV = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeW = AddressMode::kClampToEdge;
        samplerDescriptor.magFilter = FilterMode::kLinear;
        samplerDescriptor.minFilter = FilterMode::kLinear;
        samplerDescriptor.mipmapFilter = MipmapFilterMode::kLinear;
        samplerDescriptor.lodMin = 0.0f;
        samplerDescriptor.lodMax = static_cast<float>(m_offscreen.positionColorAttachmentTexture->getMipLevels());

        m_composition.positionSampler = m_device->createSampler(samplerDescriptor);

        positionSamplerBinding.index = 0;
        positionSamplerBinding.sampler = m_composition.positionSampler.get();
        positionSamplerBinding.textureView = m_offscreen.positionColorAttachmentTextureView.get();
    }

    SamplerBinding normalSamplerBinding{};
    {
        SamplerDescriptor samplerDescriptor{};
        samplerDescriptor.addressModeU = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeV = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeW = AddressMode::kClampToEdge;
        samplerDescriptor.magFilter = FilterMode::kLinear;
        samplerDescriptor.minFilter = FilterMode::kLinear;
        samplerDescriptor.mipmapFilter = MipmapFilterMode::kLinear;
        samplerDescriptor.lodMin = 0.0f;
        samplerDescriptor.lodMax = static_cast<float>(m_offscreen.normalColorAttachmentTexture->getMipLevels());

        m_composition.normalSampler = m_device->createSampler(samplerDescriptor);

        normalSamplerBinding.index = 1;
        normalSamplerBinding.sampler = m_composition.normalSampler.get();
        normalSamplerBinding.textureView = m_offscreen.normalColorAttachmentTextureView.get();
    }

    BindingGroupDescriptor descriptor{};
    descriptor.layout = m_composition.bindingGroupLayout.get();
    descriptor.samplers = { positionSamplerBinding, normalSamplerBinding };

    m_composition.bindingGroup = m_device->createBindingGroup(descriptor);
}

void DeferredSample::createCompositionPipelineLayout()
{
    PipelineLayoutDescriptor descriptor{};
    descriptor.layouts = { m_composition.bindingGroupLayout.get() };

    m_composition.pipelineLayout = m_device->createPipelineLayout(descriptor);
}

void DeferredSample::createCompositionPipeline()
{
    // Input Assembly
    InputAssemblyStage inputAssemblyStage{};
    inputAssemblyStage.topology = PrimitiveTopology::kTriangleList;

    // Vertex
    std::unique_ptr<ShaderModule> vertexShaderModule = nullptr;

    VertexStage vertexStage{};
    vertexStage.entryPoint = "main";
    { // vertex layout
        VertexInputLayout vertexInputLayout{};
        { // vertex attribute
            std::vector<VertexAttribute> attributes(2);

            VertexAttribute positionAttribute{};
            positionAttribute.format = VertexFormat::kSFLOATx3;
            positionAttribute.offset = offsetof(CompositionVertex, position);
            attributes[0] = positionAttribute;

            VertexAttribute texCoordAttribute{};
            texCoordAttribute.format = VertexFormat::kSFLOATx2;
            texCoordAttribute.offset = offsetof(CompositionVertex, textureCoordinate);
            attributes[1] = texCoordAttribute;

            vertexInputLayout.attributes = attributes;
        }
        vertexInputLayout.mode = VertexMode::kVertex;
        vertexInputLayout.stride = sizeof(CompositionVertex);

        vertexStage.layouts = { vertexInputLayout };
    }
    { // vertex shader module
        std::vector<char> vertexSource = utils::readFile(m_appDir / "composition.vert.spv", m_handle);

        ShaderModuleDescriptor shaderModuleDescriptor{};
        shaderModuleDescriptor.code = vertexSource.data();
        shaderModuleDescriptor.codeSize = static_cast<uint32_t>(vertexSource.size());
        vertexShaderModule = m_device->createShaderModule(shaderModuleDescriptor);

        vertexStage.shaderModule = vertexShaderModule.get();
    }

    // Rasterization
    RasterizationStage rasterizationStage{};
    rasterizationStage.cullMode = CullMode::kBack;
    rasterizationStage.frontFace = FrontFace::kCounterClockwise;
    rasterizationStage.sampleCount = m_sampleCount;

    // Fragment
    std::unique_ptr<ShaderModule> fragmentShaderModule = nullptr;

    FragmentStage fragmentStage{};
    fragmentStage.entryPoint = "main";
    { // fragment shader targets
        FragmentStage::Target target{};
        target.format = m_swapchain->getTextureFormat();

        fragmentStage.targets = { target };
    }

    { // fragment shader module
        std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "composition.frag.spv", m_handle);

        ShaderModuleDescriptor shaderModuleDescriptor{};
        shaderModuleDescriptor.code = fragmentShaderSource.data();
        shaderModuleDescriptor.codeSize = fragmentShaderSource.size();
        fragmentShaderModule = m_device->createShaderModule(shaderModuleDescriptor);

        fragmentStage.shaderModule = fragmentShaderModule.get();
    }

    // DepthStencil
    DepthStencilStage depthStencilStage{};
    depthStencilStage.format = m_composition.depthStencilTexture->getFormat();

    RenderPipelineDescriptor renderPipelineDescriptor{};
    renderPipelineDescriptor.inputAssembly = inputAssemblyStage;
    renderPipelineDescriptor.vertex = vertexStage;
    renderPipelineDescriptor.rasterization = rasterizationStage;
    renderPipelineDescriptor.fragment = fragmentStage;
    renderPipelineDescriptor.depthStencil = depthStencilStage;
    renderPipelineDescriptor.layout = m_composition.pipelineLayout.get();

    m_composition.pipeline = m_device->createRenderPipeline(renderPipelineDescriptor);
}

void DeferredSample::createCompositionVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = sizeof(CompositionVertex) * m_composition.vertices.size();
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_composition.vertexBuffer = m_device->createBuffer(descriptor);

    auto& vertexBuffer = m_composition.vertexBuffer;
    void* mappedPointer = vertexBuffer->map();
    memcpy(mappedPointer, m_composition.vertices.data(), descriptor.size);
    // vertexBuffer->unmap();
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