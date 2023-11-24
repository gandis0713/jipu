

#include "offscreen_sample.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace jipu
{

OffscreenSample::OffscreenSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

OffscreenSample::~OffscreenSample()
{
    clearImGui();

    m_renderPipeline.reset();
    m_renderPipelineLayout.reset();
    m_bindingGroup.reset();
    m_bindingGroupLayout.reset();
    m_vertexBuffer.reset();
    m_indexBuffer.reset();
    m_uniformBuffer.reset();
    m_queue.reset();
    m_commandBuffer.reset();
    m_swapchain.reset();
    m_device.reset();
    m_surface.reset();
    m_physicalDevice.reset();
    m_driver.reset();
}

void OffscreenSample::init()
{
    createDevier();
    createPhysicalDevice();
    createSurface();
    createDevice();
    createSwapchain();
    createCommandBuffer();
    createQueue();

    createCamera(); // need size and aspect ratio from swapchain.

    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffer();
    createBindingGroupLayout();
    createBindingGroup();
    createRenderPipeline();

    initImGui(m_device.get(), m_queue.get(), m_swapchain.get());

    m_initialized = true;
}

void OffscreenSample::createCamera()
{
    m_camera = std::make_unique<PerspectiveCamera>(45.0f,
                                                   m_swapchain->getWidth() / static_cast<float>(m_swapchain->getHeight()),
                                                   0.1f,
                                                   1000.0f);

    // auto halfWidth = m_swapchain->getWidth() / 2.0f;
    // auto halfHeight = m_swapchain->getHeight() / 2.0f;
    // m_camera = std::make_unique<OrthographicCamera>(-halfWidth, halfWidth,
    //                                                 -halfHeight, halfHeight,
    //                                                 -1000, 1000);

    m_camera->lookAt(glm::vec3(0.0f, 0.0f, 1000.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0));
}

void OffscreenSample::updateUniformBuffer()
{
    m_ubo.mvp.model = glm::mat4(1.0f);
    m_ubo.mvp.view = m_camera->getViewMat();
    m_ubo.mvp.proj = m_camera->getProjectionMat();

    void* pointer = m_uniformBuffer->map(); // do not unmap.
    memcpy(pointer, &m_ubo, m_uniformBuffer->getSize());
}

void OffscreenSample::update()
{
    updateUniformBuffer();

    updateImGui();
    buildImGui();
}

void OffscreenSample::draw()
{
    auto swapchainIndex = m_swapchain->acquireNextTexture();
    auto renderView = m_swapchain->getTextureView(swapchainIndex);
    {
        ColorAttachment attachment{};
        attachment.clearValue = { .float32 = { 0.0, 0.0, 0.0, 0.0 } };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;
        attachment.renderView = renderView;
        attachment.resolveView = nullptr;

        RenderPassEncoderDescriptor renderPassDescriptor;
        renderPassDescriptor.sampleCount = m_sampleCount;
        renderPassDescriptor.colorAttachments = { attachment };

        CommandEncoderDescriptor commandDescriptor{};
        auto commadEncoder = m_commandBuffer->createCommandEncoder(commandDescriptor);

        auto renderPassEncoder = commadEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(m_renderPipeline.get());
        renderPassEncoder->setBindingGroup(0, m_bindingGroup.get());
        renderPassEncoder->setVertexBuffer(0, m_vertexBuffer.get());
        renderPassEncoder->setIndexBuffer(m_indexBuffer.get(), IndexFormat::kUint16);
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
        renderPassEncoder->end();

        drawImGui(commadEncoder.get(), renderView);

        m_queue->submit({ commadEncoder->finish() }, m_swapchain.get());
    }
}

void OffscreenSample::updateImGui()
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
    debugWindow();
    ImGui::Render();
}

void OffscreenSample::createDevier()
{
    DriverDescriptor descriptor{};
    descriptor.type = DriverType::VULKAN;

    m_driver = Driver::create(descriptor);
}

void OffscreenSample::createPhysicalDevice()
{
    PhysicalDeviceDescriptor descriptor{};
    descriptor.index = 0; // TODO: select device.

    m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
}

void OffscreenSample::createDevice()
{
    SurfaceDescriptor descriptor{};
    descriptor.windowHandle = getWindowHandle();

    m_surface = m_driver->createSurface(descriptor);
}

void OffscreenSample::createSurface()
{
    DeviceDescriptor descriptor{};

    m_device = m_physicalDevice->createDevice(descriptor);
}

void OffscreenSample::createSwapchain()
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

void OffscreenSample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    descriptor.usage = CommandBufferUsage::kOneTime;

    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void OffscreenSample::createQueue()
{
    QueueDescriptor descriptor{};
    descriptor.flags = QueueFlagBits::kGraphics;

    m_queue = m_device->createQueue(descriptor);
}

void OffscreenSample::createVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_vertices.size() * sizeof(Vertex);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_vertexBuffer->map();
    memcpy(pointer, m_vertices.data(), descriptor.size);
    m_vertexBuffer->unmap();
}

void OffscreenSample::createIndexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_indices.size() * sizeof(uint16_t);
    descriptor.usage = BufferUsageFlagBits::kIndex;

    m_indexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_indexBuffer->map();
    memcpy(pointer, m_indices.data(), descriptor.size);
    m_indexBuffer->unmap();
}

void OffscreenSample::createUniformBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = sizeof(UBO);
    descriptor.usage = BufferUsageFlagBits::kUniform;

    m_uniformBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_uniformBuffer->map();
    // memcpy(pointer, &m_ubo, descriptor.size);
    // m_uniformBuffer->unmap();
}

void OffscreenSample::createBindingGroupLayout()
{
    BufferBindingLayout bufferLayout{};
    bufferLayout.index = 0;
    bufferLayout.stages = BindingStageFlagBits::kVertexStage;
    bufferLayout.type = BufferBindingType::kUniform;

    BindingGroupLayoutDescriptor descriptor{};
    descriptor.buffers = { bufferLayout };

    m_bindingGroupLayout = m_device->createBindingGroupLayout(descriptor);
}

void OffscreenSample::createBindingGroup()
{
    BufferBinding bufferBinding{};
    bufferBinding.buffer = m_uniformBuffer.get();
    bufferBinding.index = 0;
    bufferBinding.offset = 0;
    bufferBinding.size = m_uniformBuffer->getSize();

    BindingGroupDescriptor descriptor{};
    descriptor.layout = { m_bindingGroupLayout.get() };
    descriptor.buffers = { bufferBinding };

    m_bindingGroup = m_device->createBindingGroup(descriptor);
}

void OffscreenSample::createRenderPipeline()
{
    // render pipeline layout
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_bindingGroupLayout.get() };

        m_renderPipelineLayout = m_device->createPipelineLayout(descriptor);
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
        std::vector<char> vertexShaderSource = utils::readFile(m_appDir / "offscreen.vert.spv", m_handle);
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
        positionAttribute.location = 0;

        VertexAttribute colorAttribute{};
        colorAttribute.format = VertexFormat::kSFLOATx3;
        colorAttribute.offset = offsetof(Vertex, color);
        colorAttribute.location = 1;

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
        std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "offscreen.frag.spv", m_handle);
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
    descriptor.layout = m_renderPipelineLayout.get();

    m_renderPipeline = m_device->createRenderPipeline(descriptor);
}

} // namespace jipu
