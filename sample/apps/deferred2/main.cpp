#include <math.h>
#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <stdexcept>

#include "camera.h"
#include "file.h"
#include "im_gui.h"
#include "jipu/buffer.h"
#include "jipu/command_buffer.h"
#include "jipu/command_encoder.h"
#include "jipu/device.h"
#include "jipu/driver.h"
#include "jipu/physical_device.h"
#include "jipu/pipeline.h"
#include "jipu/pipeline_layout.h"
#include "jipu/queue.h"
#include "jipu/render_pass_encoder.h"
#include "jipu/surface.h"
#include "jipu/swapchain.h"
#include "khronos_texture.h"
#include "light.h"
#include "model.h"
#include "sample.h"

namespace jipu
{

class Deferred2Sample : public Sample, public Im_Gui
{
public:
    Deferred2Sample() = delete;
    Deferred2Sample(const SampleDescriptor& descriptor);
    ~Deferred2Sample() override;

public:
    void init() override;
    void update() override;
    void draw() override;

private:
    void updateImGui() override;

private:
    void createDriver();
    void getPhysicalDevices();
    void createSurface();
    void createDevice();
    void createSwapchain();

    void createOffscreenPositionColorAttachmentTexture();
    void createOffscreenPositionColorAttachmentTextureView();
    void createOffscreenNormalColorAttachmentTexture();
    void createOffscreenNormalColorAttachmentTextureView();
    void createOffscreenAlbedoColorAttachmentTexture();
    void createOffscreenAlbedoColorAttachmentTextureView();
    void createOffscreenDepthStencilTexture();
    void createOffscreenDepthStencilTextureView();
    void createOffscreenColorMapTexture();
    void createOffscreenColorMapTextureView();
    void createOffscreenNormalMapTexture();
    void createOffscreenNormalMapTextureView();
    void createOffscreenCamera();
    void createOffscreenUniformBuffer();
    void createOffscreenVertexBuffer();
    void createOffscreenBindingGroupLayout();
    void createOffscreenBindingGroup();
    void createOffscreenPipelineLayout();
    void createOffscreenPipeline();
    void updateOffscreenUniformBuffer();

    void createCompositionDepthStencilTexture();
    void createCompositionDepthStencilTextureView();
    void createCompositionBindingGroupLayout();
    void createCompositionBindingGroup();
    void createCompositionPipelineLayout();
    void createCompositionPipeline();
    void createCompositionUniformBuffer();
    void createCompositionVertexBuffer();
    void updateCompositionUniformBuffer();

    void createCommandBuffer();
    void createQueue();

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::vector<std::unique_ptr<PhysicalDevice>> m_physicalDevices{};
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;

    struct CompositionUBO
    {
        struct Light
        {
            alignas(16) glm::vec3 position;
            alignas(16) glm::vec3 color;
        };

        std::vector<CompositionUBO::Light> lights{};
        alignas(16) glm::vec3 cameraPosition;
        int lightCount = 8;
        int padding1;
        int padding2;
        int padding3;
    };

    struct MVP
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    } m_mvp;

    struct
    {
        std::unique_ptr<Texture> positionColorAttachmentTexture = nullptr;
        std::unique_ptr<TextureView> positionColorAttachmentTextureView = nullptr;
        std::unique_ptr<Texture> normalColorAttachmentTexture = nullptr;
        std::unique_ptr<TextureView> normalColorAttachmentTextureView = nullptr;
        std::unique_ptr<Texture> albedoColorAttachmentTexture = nullptr;
        std::unique_ptr<TextureView> albedoColorAttachmentTextureView = nullptr;
        std::unique_ptr<Texture> depthStencilTexture = nullptr;
        std::unique_ptr<TextureView> depthStencilTextureView = nullptr;
        std::unique_ptr<Texture> colorMapTexture = nullptr;
        std::unique_ptr<TextureView> colorMapTextureView = nullptr;
        std::unique_ptr<Texture> normalMapTexture = nullptr;
        std::unique_ptr<TextureView> normalMapTextureView = nullptr;
        std::unique_ptr<Sampler> colorMapSampler = nullptr;
        std::unique_ptr<Sampler> normalMapSampler = nullptr;
        std::unique_ptr<Buffer> uniformBuffer = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        std::unique_ptr<Buffer> indexBuffer = nullptr;
        std::unique_ptr<BindingGroupLayout> bindingGroupLayout = nullptr;
        std::unique_ptr<BindingGroup> bindingGroup = nullptr;
        std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
        std::unique_ptr<Pipeline> pipeline = nullptr;
        std::unique_ptr<Camera> camera = nullptr;
        Polygon polygon{};
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
        std::unique_ptr<Sampler> albedoSampler = nullptr;
        std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
        std::unique_ptr<Pipeline> pipeline = nullptr;
        std::unique_ptr<Buffer> uniformBuffer = nullptr;
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        CompositionUBO ubo{};
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
    int m_lightMax = 10000;
};

Deferred2Sample::Deferred2Sample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
    // Do not call init function.
}

Deferred2Sample::~Deferred2Sample()
{
    clearImGui();

    m_composition.pipeline.reset();
    m_composition.pipelineLayout.reset();
    m_composition.bindingGroupLayout.reset();
    m_composition.bindingGroup.reset();
    m_composition.vertexBuffer.reset();
    m_composition.uniformBuffer.reset();
    m_composition.positionSampler.reset();
    m_composition.normalSampler.reset();
    m_composition.albedoSampler.reset();
    m_composition.depthStencilTextureView.reset();
    m_composition.depthStencilTexture.reset();

    m_offscreen.pipeline.reset();
    m_offscreen.pipelineLayout.reset();
    m_offscreen.bindingGroupLayout.reset();
    m_offscreen.bindingGroup.reset();
    m_offscreen.camera.reset();
    m_offscreen.vertexBuffer.reset();
    m_offscreen.indexBuffer.reset();
    m_offscreen.uniformBuffer.reset();
    m_offscreen.normalMapSampler.reset();
    m_offscreen.colorMapSampler.reset();
    m_offscreen.colorMapTextureView.reset();
    m_offscreen.colorMapTexture.reset();
    m_offscreen.normalMapTextureView.reset();
    m_offscreen.normalMapTexture.reset();
    m_offscreen.depthStencilTextureView.reset();
    m_offscreen.depthStencilTexture.reset();
    m_offscreen.albedoColorAttachmentTextureView.reset();
    m_offscreen.albedoColorAttachmentTexture.reset();
    m_offscreen.normalColorAttachmentTextureView.reset();
    m_offscreen.normalColorAttachmentTexture.reset();
    m_offscreen.positionColorAttachmentTextureView.reset();
    m_offscreen.positionColorAttachmentTexture.reset();

    m_queue.reset();
    m_commandBuffer.reset();

    m_swapchain.reset();
    m_device.reset();

    m_surface.reset();
    m_physicalDevices.clear();
    m_driver.reset();
}

void Deferred2Sample::init()
{
    createDriver();
    getPhysicalDevices();
    createSurface();
    createDevice();
    createSwapchain();

    createCommandBuffer();
    createQueue();

    createOffscreenPositionColorAttachmentTexture();
    createOffscreenPositionColorAttachmentTextureView();
    createOffscreenNormalColorAttachmentTexture();
    createOffscreenNormalColorAttachmentTextureView();
    createOffscreenAlbedoColorAttachmentTexture();
    createOffscreenAlbedoColorAttachmentTextureView();
    createOffscreenDepthStencilTexture();
    createOffscreenDepthStencilTextureView();
    createOffscreenColorMapTexture();
    createOffscreenColorMapTextureView();
    createOffscreenNormalMapTexture();
    createOffscreenNormalMapTextureView();
    createOffscreenCamera();
    createOffscreenUniformBuffer();
    createOffscreenVertexBuffer();
    createOffscreenBindingGroupLayout();
    createOffscreenBindingGroup();
    createOffscreenPipelineLayout();
    createOffscreenPipeline();

    createCompositionDepthStencilTexture();
    createCompositionDepthStencilTextureView();
    createCompositionUniformBuffer();
    createCompositionVertexBuffer();
    createCompositionBindingGroupLayout();
    createCompositionBindingGroup();
    createCompositionPipelineLayout();
    createCompositionPipeline();

    initImGui(m_device.get(), m_queue.get(), m_swapchain.get());

    m_initialized = true;
}

void Deferred2Sample::update()
{
    updateOffscreenUniformBuffer();
    updateCompositionUniformBuffer();

    updateImGui();
    buildImGui();
}

void Deferred2Sample::updateOffscreenUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();

    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() / 5.0f;

    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 80.0f));
    glm::mat4 R1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 R2 = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 reori = R2 * R1 * T;
    m_mvp.model = reori;

    void* mappedPointer = m_offscreen.uniformBuffer->map();
    memcpy(mappedPointer, &m_mvp, sizeof(MVP));
}

void Deferred2Sample::updateCompositionUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();

    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    float ratio = sin(time);
    for (auto i = 0; i < m_lightMax; ++i)
    {
        auto& light = m_composition.ubo.lights[i];
        light.position[0] = light.position[0] * ratio;
        light.position[1] = light.position[1] * ratio;
        // light.position[2] = light.position[2] * ratio;
    }
    m_composition.ubo.cameraPosition = m_offscreen.camera->getPosition();

    uint32_t lightSize = static_cast<uint32_t>(sizeof(CompositionUBO::Light) * m_lightMax);
    uint32_t cameraPositionSize = 16; // alignas(16) glm::vec3

    auto& uniformBuffer = m_composition.uniformBuffer;
    void* pointer = uniformBuffer->map();
    char* bytePointer = static_cast<char*>(pointer);
    memcpy(bytePointer, m_composition.ubo.lights.data(), lightSize);
    bytePointer += lightSize;
    memcpy(bytePointer, &m_composition.ubo.cameraPosition, cameraPositionSize);
    bytePointer += cameraPositionSize;
    memcpy(bytePointer, &m_composition.ubo.lightCount, sizeof(int));
}

void Deferred2Sample::updateImGui()
{
    // set display size and mouse state.
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)m_width, (float)m_height);
        io.MousePos = ImVec2(m_mouseX, m_mouseY);
        io.MouseDown[0] = m_leftMouseButton;
        io.MouseDown[1] = m_rightMouseButton;
        io.MouseDown[2] = m_middleMouseButton;
    }

    ImGui::NewFrame();

    // set windows position and size
    {
        auto scale = ImGui::GetIO().FontGlobalScale;
        ImGui::SetNextWindowPos(ImVec2(20, 20 + m_padding.top), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300 * scale, 100 * scale), ImGuiCond_FirstUseEver);
    }

    // set ui
    {
        ImGui::Begin("Settings");
        ImGui::SliderInt("Number of Light", &m_composition.ubo.lightCount, 1, m_lightMax);
        ImGui::End();
    }

    debugWindow();
    ImGui::Render();
}

void Deferred2Sample::draw()
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

        ColorAttachment albedoColorAttachment{};
        albedoColorAttachment.loadOp = LoadOp::kClear;
        albedoColorAttachment.storeOp = StoreOp::kStore;
        albedoColorAttachment.renderView = m_offscreen.albedoColorAttachmentTextureView.get();
        albedoColorAttachment.resolveView = nullptr;
        albedoColorAttachment.clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };

        DepthStencilAttachment depthStencilAttachment{};
        depthStencilAttachment.textureView = m_offscreen.depthStencilTextureView.get();
        depthStencilAttachment.depthLoadOp = LoadOp::kClear;
        depthStencilAttachment.depthStoreOp = StoreOp::kStore;
        depthStencilAttachment.clearValue = { .depth = 1.0f, .stencil = 0 };

        RenderPassEncoderDescriptor renderPassEncoderDescriptor{};
        renderPassEncoderDescriptor.colorAttachments = { positionColorAttachment, normalColorAttachment, albedoColorAttachment };
        renderPassEncoderDescriptor.depthStencilAttachment = depthStencilAttachment;
        renderPassEncoderDescriptor.sampleCount = m_sampleCount;

        auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassEncoderDescriptor);
        renderPassEncoder->setPipeline(m_offscreen.pipeline.get());
        renderPassEncoder->setVertexBuffer(0, m_offscreen.vertexBuffer.get());
        renderPassEncoder->setIndexBuffer(m_offscreen.indexBuffer.get(), IndexFormat::kUint16);
        renderPassEncoder->setBindingGroup(0, m_offscreen.bindingGroup.get());
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_offscreen.polygon.indices.size()), 1, 0, 0, 0);
        renderPassEncoder->end();
    }
    // second pass
    {
        int targetIndex = m_swapchain->acquireNextTexture();
        auto renderView = m_swapchain->getTextureViews()[targetIndex];

        ColorAttachment colorAttachment{};
        colorAttachment.loadOp = LoadOp::kClear;
        colorAttachment.storeOp = StoreOp::kStore;
        colorAttachment.renderView = renderView;
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
        renderPassEncoder->setVertexBuffer(0, m_composition.vertexBuffer.get());
        renderPassEncoder->setBindingGroup(0, m_composition.bindingGroup.get());
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->draw(static_cast<uint32_t>(m_composition.vertices.size()));
        renderPassEncoder->end();

        drawImGui(commandEncoder.get(), renderView);

        // renderPassEncoder->end();
    }

    m_queue->submit({ commandEncoder->finish() }, m_swapchain.get());
}

void Deferred2Sample::createDriver()
{
    DriverDescriptor descriptor;
    descriptor.type = DriverType::kVulkan;
    m_driver = Driver::create(descriptor);
}

void Deferred2Sample::getPhysicalDevices()
{
    m_physicalDevices = m_driver->getPhysicalDevices();
}

void Deferred2Sample::createSurface()
{
    SurfaceDescriptor descriptor;
    descriptor.windowHandle = getWindowHandle();
    m_surface = m_driver->createSurface(descriptor);
}

void Deferred2Sample::createSwapchain()
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

void Deferred2Sample::createDevice()
{
    // TODO: select suit device.
    PhysicalDevice* physicalDevice = m_physicalDevices[0].get();

    DeviceDescriptor descriptor;
    m_device = physicalDevice->createDevice(descriptor);
}

void Deferred2Sample::createOffscreenPositionColorAttachmentTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kRGBA_16161616_UInt_Norm;
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.depth = 1;
    descriptor.usage = TextureUsageFlagBits::kColorAttachment | TextureUsageFlagBits::kTextureBinding;

    m_offscreen.positionColorAttachmentTexture = m_device->createTexture(descriptor);
}

void Deferred2Sample::createOffscreenPositionColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.positionColorAttachmentTextureView = m_offscreen.positionColorAttachmentTexture->createTextureView(descriptor);
}

void Deferred2Sample::createOffscreenNormalColorAttachmentTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kRGBA_16161616_UInt_Norm;
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.depth = 1;
    descriptor.usage = TextureUsageFlagBits::kColorAttachment |
                       TextureUsageFlagBits::kTextureBinding;

    m_offscreen.normalColorAttachmentTexture = m_device->createTexture(descriptor);
}

void Deferred2Sample::createOffscreenNormalColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.normalColorAttachmentTextureView = m_offscreen.normalColorAttachmentTexture->createTextureView(descriptor);
}

void Deferred2Sample::createOffscreenAlbedoColorAttachmentTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kBGRA_8888_UInt_Norm;
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.depth = 1;
    descriptor.usage = TextureUsageFlagBits::kColorAttachment | TextureUsageFlagBits::kTextureBinding;

    m_offscreen.albedoColorAttachmentTexture = m_device->createTexture(descriptor);
}

void Deferred2Sample::createOffscreenAlbedoColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.albedoColorAttachmentTextureView = m_offscreen.albedoColorAttachmentTexture->createTextureView(descriptor);
}

void Deferred2Sample::createOffscreenDepthStencilTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kD_32_SFloat;
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.depth = 1;
    descriptor.usage = TextureUsageFlagBits::kDepthStencil;

    m_offscreen.depthStencilTexture = m_device->createTexture(descriptor);
}

void Deferred2Sample::createOffscreenDepthStencilTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kDepth;

    m_offscreen.depthStencilTextureView = m_offscreen.depthStencilTexture->createTextureView(descriptor);
}

void Deferred2Sample::createOffscreenColorMapTexture()
{
    // ktx{ m_appDir / "colormap_rgba.ktx" };
    std::vector<char> data = utils::readFile(m_appDir / "colormap_rgba.ktx", m_handle);
    KTX ktx{ data.data(), data.size() };

    TextureDescriptor textureDescriptor{};
    textureDescriptor.type = TextureType::k2D;
    textureDescriptor.format = TextureFormat::kRGBA_8888_UInt_Norm; // kRGBA_8888_UInt_Norm_SRGB
    textureDescriptor.mipLevels = 1;
    textureDescriptor.sampleCount = 1;
    textureDescriptor.width = ktx.getWidth();
    textureDescriptor.height = ktx.getHeight();
    textureDescriptor.depth = 1;
    textureDescriptor.usage = TextureUsageFlagBits::kCopySrc |
                              TextureUsageFlagBits::kCopyDst |
                              TextureUsageFlagBits::kTextureBinding,

    m_offscreen.colorMapTexture = m_device->createTexture(textureDescriptor);

    // copy texture data
    {
        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = ktx.getWidth() * ktx.getHeight() * ktx.getChannel() * sizeof(char);
        bufferDescriptor.usage = BufferUsageFlagBits::kCopySrc;

        auto stagingBuffer = m_device->createBuffer(bufferDescriptor);
        void* pointer = stagingBuffer->map();
        memcpy(pointer, ktx.getPixels(), bufferDescriptor.size);
        // stagingBuffer->unmap();

        BlitTextureBuffer blitTextureBuffer{};
        blitTextureBuffer.buffer = stagingBuffer.get();
        blitTextureBuffer.bytesPerRow = ktx.getWidth() * ktx.getChannel() * sizeof(char);
        blitTextureBuffer.rowsPerTexture = ktx.getHeight();
        blitTextureBuffer.offset = 0;

        BlitTexture blitTexture{};
        blitTexture.texture = m_offscreen.colorMapTexture.get();
        blitTexture.aspect = TextureAspectFlagBits::kColor;

        Extent3D extent{};
        extent.width = ktx.getWidth();
        extent.height = ktx.getHeight();
        extent.depth = 1;

        CommandBufferDescriptor commandBufferDescriptor{};
        commandBufferDescriptor.usage = CommandBufferUsage::kOneTime;

        auto commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);

        CommandEncoderDescriptor commandEncoderDescriptor{};
        auto commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);

        commandEncoder->copyBufferToTexture(blitTextureBuffer, blitTexture, extent);
        commandEncoder->finish();

        m_queue->submit({ commandBuffer.get() });
    }
}

void Deferred2Sample::createOffscreenColorMapTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.colorMapTextureView = m_offscreen.colorMapTexture->createTextureView(descriptor);
}

void Deferred2Sample::createOffscreenNormalMapTexture()
{
    // KTX ktx{ m_appDir / "normalmap_rgba.ktx" };
    std::vector<char> data = utils::readFile(m_appDir / "normalmap_rgba.ktx", m_handle);
    KTX ktx{ data.data(), data.size() };

    TextureDescriptor textureDescriptor{};
    textureDescriptor.type = TextureType::k2D;
    textureDescriptor.format = TextureFormat::kRGBA_8888_UInt_Norm;
    textureDescriptor.mipLevels = 1;
    textureDescriptor.sampleCount = 1;
    textureDescriptor.width = ktx.getWidth();
    textureDescriptor.height = ktx.getHeight();
    textureDescriptor.depth = 1;
    textureDescriptor.usage = TextureUsageFlagBits::kCopySrc |
                              TextureUsageFlagBits::kCopyDst |
                              TextureUsageFlagBits::kTextureBinding,

    m_offscreen.normalMapTexture = m_device->createTexture(textureDescriptor);

    // copy texture data
    {
        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = ktx.getWidth() * ktx.getHeight() * ktx.getChannel() * sizeof(char);
        bufferDescriptor.usage = BufferUsageFlagBits::kCopySrc;

        auto stagingBuffer = m_device->createBuffer(bufferDescriptor);
        void* pointer = stagingBuffer->map();
        memcpy(pointer, ktx.getPixels(), bufferDescriptor.size);
        // stagingBuffer->unmap();

        BlitTextureBuffer blitTextureBuffer{};
        blitTextureBuffer.buffer = stagingBuffer.get();
        blitTextureBuffer.bytesPerRow = ktx.getWidth() * ktx.getChannel() * sizeof(char);
        blitTextureBuffer.rowsPerTexture = ktx.getHeight();
        blitTextureBuffer.offset = 0;

        BlitTexture blitTexture{};
        blitTexture.texture = m_offscreen.normalMapTexture.get();
        blitTexture.aspect = TextureAspectFlagBits::kColor;

        Extent3D extent{};
        extent.width = ktx.getWidth();
        extent.height = ktx.getHeight();
        extent.depth = 1;

        CommandBufferDescriptor commandBufferDescriptor{};
        commandBufferDescriptor.usage = CommandBufferUsage::kOneTime;

        auto commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);

        CommandEncoderDescriptor commandEncoderDescriptor{};
        auto commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);

        commandEncoder->copyBufferToTexture(blitTextureBuffer, blitTexture, extent);
        commandEncoder->finish();

        m_queue->submit({ commandBuffer.get() });
    }
}

void Deferred2Sample::createOffscreenNormalMapTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.normalMapTextureView = m_offscreen.normalMapTexture->createTextureView(descriptor);
}

void Deferred2Sample::createOffscreenCamera()
{
    m_offscreen.camera = std::make_unique<PerspectiveCamera>(glm::radians(45.0f),
                                                             m_swapchain->getWidth() / static_cast<float>(m_swapchain->getHeight()),
                                                             0.1f,
                                                             1000.0f);
    m_offscreen.camera->lookAt(glm::vec3(0.0f, 0.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

void Deferred2Sample::createOffscreenUniformBuffer()
{
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 80.0f));
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 reori = R * T;
    m_mvp.model = reori;
    m_mvp.view = m_offscreen.camera->getViewMat();
    m_mvp.proj = m_offscreen.camera->getProjectionMat();

    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = sizeof(MVP);
    bufferDescriptor.usage = BufferUsageFlagBits::kUniform;

    m_offscreen.uniformBuffer = m_device->createBuffer(bufferDescriptor);
    void* mappedPointer = m_offscreen.uniformBuffer->map();
    memcpy(mappedPointer, &m_mvp, sizeof(MVP));
    // m_offscreen.uniformBuffer->unmap();
}

void Deferred2Sample::createOffscreenVertexBuffer()
{
    // m_offscreen.polygon = loadGLTF(m_appDir / "armor.gltf");
    std::vector<char> data = utils::readFile(m_appDir / "armor.gltf", m_handle);
    m_offscreen.polygon = loadGLTF(data.data(), data.size(), m_appDir);

    {
        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = sizeof(Vertex) * m_offscreen.polygon.vertices.size();
        bufferDescriptor.usage = BufferUsageFlagBits::kVertex;

        m_offscreen.vertexBuffer = m_device->createBuffer(bufferDescriptor);
        void* mappedPointer = m_offscreen.vertexBuffer->map();
        memcpy(mappedPointer, m_offscreen.polygon.vertices.data(), bufferDescriptor.size);
        m_offscreen.vertexBuffer->unmap();
    }
    {
        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = sizeof(uint16_t) * m_offscreen.polygon.indices.size();
        bufferDescriptor.usage = BufferUsageFlagBits::kIndex;

        m_offscreen.indexBuffer = m_device->createBuffer(bufferDescriptor);
        void* mappedPointer = m_offscreen.indexBuffer->map();
        memcpy(mappedPointer, m_offscreen.polygon.indices.data(), bufferDescriptor.size);
        m_offscreen.indexBuffer->unmap();
    }
}

void Deferred2Sample::createOffscreenBindingGroupLayout()
{
    BufferBindingLayout bufferBindingLayout{};
    bufferBindingLayout.type = BufferBindingType::kUniform;
    bufferBindingLayout.index = 0;
    bufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;

    SamplerBindingLayout colorSamplerBindingLayout{};
    colorSamplerBindingLayout.index = 1;
    colorSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
    colorSamplerBindingLayout.withTexture = true;

    SamplerBindingLayout normalSamplerBindingLayout{};
    normalSamplerBindingLayout.index = 2;
    normalSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
    normalSamplerBindingLayout.withTexture = true;

    BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
    bindingGroupLayoutDescriptor.buffers = { bufferBindingLayout };
    bindingGroupLayoutDescriptor.samplers = { colorSamplerBindingLayout, normalSamplerBindingLayout };

    m_offscreen.bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
}

void Deferred2Sample::createOffscreenBindingGroup()
{
    BufferBinding bufferBinding{};
    bufferBinding.buffer = m_offscreen.uniformBuffer.get();
    bufferBinding.index = 0;
    bufferBinding.offset = 0;
    bufferBinding.size = sizeof(MVP);

    { // create color map sampler

        SamplerDescriptor samplerDescriptor{};
        samplerDescriptor.addressModeU = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeV = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeW = AddressMode::kClampToEdge;
        samplerDescriptor.lodMin = 0.0f;
        samplerDescriptor.lodMax = static_cast<float>(m_offscreen.colorMapTexture->getMipLevels());
        samplerDescriptor.minFilter = FilterMode::kLinear;
        samplerDescriptor.magFilter = FilterMode::kLinear;
        samplerDescriptor.mipmapFilter = MipmapFilterMode::kLinear;

        m_offscreen.colorMapSampler = m_device->createSampler(samplerDescriptor);
    }

    { // create normal map sampler

        SamplerDescriptor samplerDescriptor{};
        samplerDescriptor.addressModeU = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeV = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeW = AddressMode::kClampToEdge;
        samplerDescriptor.lodMin = 0.0f;
        samplerDescriptor.lodMax = static_cast<float>(m_offscreen.normalMapTexture->getMipLevels());
        samplerDescriptor.minFilter = FilterMode::kLinear;
        samplerDescriptor.magFilter = FilterMode::kLinear;
        samplerDescriptor.mipmapFilter = MipmapFilterMode::kLinear;

        m_offscreen.normalMapSampler = m_device->createSampler(samplerDescriptor);
    }

    SamplerBinding colorSamplerBinding{};
    colorSamplerBinding.index = 1;
    colorSamplerBinding.sampler = m_offscreen.colorMapSampler.get();
    colorSamplerBinding.textureView = m_offscreen.colorMapTextureView.get();

    SamplerBinding normalSamplerBinding{};
    normalSamplerBinding.index = 2;
    normalSamplerBinding.sampler = m_offscreen.normalMapSampler.get();
    normalSamplerBinding.textureView = m_offscreen.normalMapTextureView.get();

    BindingGroupDescriptor bindingGroupDescriptor{};
    bindingGroupDescriptor.buffers = { bufferBinding };
    bindingGroupDescriptor.samplers = { colorSamplerBinding, normalSamplerBinding };
    bindingGroupDescriptor.layout = m_offscreen.bindingGroupLayout.get();

    m_offscreen.bindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
}

void Deferred2Sample::createOffscreenPipelineLayout()
{
    PipelineLayoutDescriptor descriptor{};
    descriptor.layouts = { m_offscreen.bindingGroupLayout.get() };
    m_offscreen.pipelineLayout = m_device->createPipelineLayout(descriptor);
}

void Deferred2Sample::createOffscreenPipeline()
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
        inputLayout.stride = sizeof(Vertex);

        VertexAttribute positionAttribute;
        positionAttribute.format = VertexFormat::kSFLOATx3;
        positionAttribute.offset = offsetof(Vertex, pos);
        positionAttribute.location = 0;

        VertexAttribute normalAttribute;
        normalAttribute.format = VertexFormat::kSFLOATx3;
        normalAttribute.offset = offsetof(Vertex, normal);
        normalAttribute.location = 1;

        VertexAttribute tangentAttribute;
        tangentAttribute.format = VertexFormat::kSFLOATx4;
        tangentAttribute.offset = offsetof(Vertex, tangent);
        tangentAttribute.location = 2;

        VertexAttribute texCoordAttribute;
        texCoordAttribute.format = VertexFormat::kSFLOATx2;
        texCoordAttribute.offset = offsetof(Vertex, texCoord);
        texCoordAttribute.location = 3;

        inputLayout.attributes = { positionAttribute, normalAttribute, tangentAttribute, texCoordAttribute };

        vertexShage.layouts = { inputLayout };

        vertexShage.shaderModule = vertexShaderModule.get();
    }

    // Rasterization
    RasterizationStage rasterizationStage{};
    rasterizationStage.sampleCount = m_sampleCount;
    rasterizationStage.cullMode = CullMode::kNone;
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
        positionTarget.format = m_offscreen.positionColorAttachmentTexture->getFormat();

        FragmentStage::Target normalTarget{};
        normalTarget.format = m_offscreen.normalColorAttachmentTexture->getFormat();

        FragmentStage::Target albedoTarget{};
        albedoTarget.format = m_offscreen.albedoColorAttachmentTexture->getFormat();

        fragmentStage.targets = { positionTarget, normalTarget, albedoTarget };
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

void Deferred2Sample::createCompositionDepthStencilTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kD_32_SFloat;
    descriptor.usage = TextureUsageFlagBits::kDepthStencil;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.depth = 1;
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;

    m_composition.depthStencilTexture = m_device->createTexture(descriptor);
}

void Deferred2Sample::createCompositionDepthStencilTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kDepth;

    m_composition.depthStencilTextureView = m_offscreen.depthStencilTexture->createTextureView(descriptor);
}

void Deferred2Sample::createCompositionBindingGroupLayout()
{
    BindingGroupLayoutDescriptor descriptor{};

    SamplerBindingLayout positionSamplerBindingLayout{};
    positionSamplerBindingLayout.index = 0;
    positionSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
    positionSamplerBindingLayout.withTexture = true;

    SamplerBindingLayout normalSamplerBindingLayout{};
    normalSamplerBindingLayout.index = 1;
    normalSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
    normalSamplerBindingLayout.withTexture = true;

    SamplerBindingLayout albedoSamplerBindingLayout{};
    albedoSamplerBindingLayout.index = 2;
    albedoSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
    albedoSamplerBindingLayout.withTexture = true;

    BufferBindingLayout uniformBufferBindingLayout{};
    uniformBufferBindingLayout.index = 3;
    uniformBufferBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
    uniformBufferBindingLayout.type = BufferBindingType::kUniform;

    descriptor.buffers = { uniformBufferBindingLayout };
    descriptor.samplers = { positionSamplerBindingLayout, normalSamplerBindingLayout, albedoSamplerBindingLayout };

    m_composition.bindingGroupLayout = m_device->createBindingGroupLayout(descriptor);
}

void Deferred2Sample::createCompositionBindingGroup()
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

    SamplerBinding albedoSamplerBinding{};
    {
        SamplerDescriptor samplerDescriptor{};
        samplerDescriptor.addressModeU = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeV = AddressMode::kClampToEdge;
        samplerDescriptor.addressModeW = AddressMode::kClampToEdge;
        samplerDescriptor.magFilter = FilterMode::kLinear;
        samplerDescriptor.minFilter = FilterMode::kLinear;
        samplerDescriptor.mipmapFilter = MipmapFilterMode::kLinear;
        samplerDescriptor.lodMin = 0.0f;
        samplerDescriptor.lodMax = static_cast<float>(m_offscreen.albedoColorAttachmentTexture->getMipLevels());

        m_composition.albedoSampler = m_device->createSampler(samplerDescriptor);

        albedoSamplerBinding.index = 2;
        albedoSamplerBinding.sampler = m_composition.albedoSampler.get();
        albedoSamplerBinding.textureView = m_offscreen.albedoColorAttachmentTextureView.get();
    }

    BufferBinding uniformBufferBinding{};
    {
        uniformBufferBinding.buffer = m_composition.uniformBuffer.get();
        uniformBufferBinding.index = 3;
        uniformBufferBinding.offset = 0;
        uniformBufferBinding.size = m_composition.uniformBuffer->getSize();
    }

    BindingGroupDescriptor descriptor{};
    descriptor.layout = m_composition.bindingGroupLayout.get();
    descriptor.buffers = { uniformBufferBinding };
    descriptor.samplers = { positionSamplerBinding, normalSamplerBinding, albedoSamplerBinding };

    m_composition.bindingGroup = m_device->createBindingGroup(descriptor);
}

void Deferred2Sample::createCompositionPipelineLayout()
{
    PipelineLayoutDescriptor descriptor{};
    descriptor.layouts = { m_composition.bindingGroupLayout.get() };

    m_composition.pipelineLayout = m_device->createPipelineLayout(descriptor);
}

void Deferred2Sample::createCompositionPipeline()
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
            positionAttribute.location = 0;
            attributes[0] = positionAttribute;

            VertexAttribute texCoordAttribute{};
            texCoordAttribute.format = VertexFormat::kSFLOATx2;
            texCoordAttribute.offset = offsetof(CompositionVertex, textureCoordinate);
            texCoordAttribute.location = 1;
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

void Deferred2Sample::createCompositionUniformBuffer()
{
    // light
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> xDirDist(0, 1);
        std::uniform_int_distribution<> yDirDist(0, 1);
        std::uniform_int_distribution<> zDirDist(0, 1);
        std::uniform_real_distribution<> xDist(30.0f, 30.0f);
        std::uniform_real_distribution<> yDist(30.0f, 30.0f);
        std::uniform_real_distribution<> zDist(30.0f, 30.0f);
        std::uniform_real_distribution<> rDist(0.0f, 1.0f);
        std::uniform_real_distribution<> gDist(0.0f, 1.0f);
        std::uniform_real_distribution<> bDist(0.0f, 1.0f);

        for (int i = 0; i < m_lightMax; i++)
        {
            auto xDir = xDirDist(gen) == 0 ? -1 : 1;
            auto yDir = yDirDist(gen) == 0 ? -1 : 1;
            auto zDir = zDirDist(gen) == 0 ? -1 : 1;
            auto x = xDist(gen);
            auto y = yDist(gen);
            auto z = zDist(gen);
            auto r = rDist(gen);
            auto g = gDist(gen);
            auto b = bDist(gen);

            // spdlog::trace("xDir: {}, yDir: {}, zDir: {}, x: {}, y: {}, z: {}", xDir, yDir, zDir, x, y, z);
            // spdlog::trace("r: {}, g: {}, b: {}", r, g, b);

            CompositionUBO::Light light{ { xDir * x, yDir * y, zDir * z },
                                         { r, g, b } };
            m_composition.ubo.lights.push_back(light);
        }
    }
    // camera position
    {
        m_composition.ubo.cameraPosition = m_offscreen.camera->getPosition();
    }

    uint32_t lightSize = static_cast<uint32_t>(sizeof(CompositionUBO::Light) * m_lightMax);
    uint32_t cameraPositionSize = 16; // alignas(16) glm::vec3

    BufferDescriptor descriptor{};
    descriptor.size = lightSize + cameraPositionSize;
    descriptor.size += sizeof(int) * 3; // padding
    descriptor.usage = BufferUsageFlagBits::kUniform;

    m_composition.uniformBuffer = m_device->createBuffer(descriptor);

    auto& uniformBuffer = m_composition.uniformBuffer;
    void* pointer = uniformBuffer->map();
    char* bytePointer = static_cast<char*>(pointer);
    memcpy(bytePointer, m_composition.ubo.lights.data(), lightSize);
    bytePointer += lightSize;
    memcpy(bytePointer, &m_composition.ubo.cameraPosition, cameraPositionSize);
    bytePointer += cameraPositionSize;
    memcpy(bytePointer, &m_composition.ubo.lightCount, sizeof(int));
    // uniformBuffer->unmap();
}

void Deferred2Sample::createCompositionVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = sizeof(CompositionVertex) * m_composition.vertices.size();
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_composition.vertexBuffer = m_device->createBuffer(descriptor);

    auto& vertexBuffer = m_composition.vertexBuffer;
    void* mappedPointer = vertexBuffer->map();
    memcpy(mappedPointer, m_composition.vertices.data(), descriptor.size);
    vertexBuffer->unmap();
}

void Deferred2Sample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    descriptor.usage = CommandBufferUsage::kOneTime;

    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void Deferred2Sample::createQueue()
{
    QueueDescriptor descriptor{};
    descriptor.flags = QueueFlagBits::kGraphics;

    m_queue = m_device->createQueue(descriptor);
}

} // namespace jipu

#if defined(__ANDROID__) || defined(ANDROID)

// GameActivity's C/C++ code
#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

// Glue from GameActivity to android_main()
// Passing GameActivity event from main thread to app native thread.
extern "C"
{
#include <game-activity/native_app_glue/android_native_app_glue.c>
}

void android_main(struct android_app* app)
{
    jipu::SampleDescriptor descriptor{
        { 1000, 2000, "Deferred2 Sample", app },
        ""
    };

    jipu::Deferred2Sample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    jipu::SampleDescriptor descriptor{
        { 800, 600, "Deferred2 Sample", nullptr },
        argv[0]
    };

    jipu::Deferred2Sample sample(descriptor);

    return sample.exec();
}

#endif