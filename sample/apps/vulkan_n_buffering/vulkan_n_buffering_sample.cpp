#include "vulkan_n_buffering_sample.h"

#include <math.h>
#include <spdlog/spdlog.h>

#include <random>
#include <stdexcept>

#include "vulkan_device.h"
#include "vulkan_swapchain.h"

namespace jipu
{

VulkanNBufferingSample::VulkanNBufferingSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
    // Do not call init function.
}

VulkanNBufferingSample::~VulkanNBufferingSample()
{
    clearImGui();

    m_depthStencilTextureView.reset();
    m_depthStencilTexture.reset();

    m_composition.vertexBuffer.reset();
    m_composition.uniformBuffer.reset();

    m_composition.vertexShaderModule.reset();
    m_composition.fragmentShaderModule.reset();
    m_composition.renderPipeline.reset();
    m_composition.pipelineLayout.reset();
    m_composition.bindingGroupLayouts.clear();
    m_composition.bindingGroups.clear();
    m_composition.albedoSampler.reset();
    m_composition.normalSampler.reset();
    m_composition.positionSampler.reset();

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

    m_offscreen.vertexShaderModule.reset();
    m_offscreen.fragmentShaderModule.reset();
    m_offscreen.renderPipeline.reset();
    m_offscreen.pipelineLayout.reset();
    m_offscreen.bindingGroupLayouts.clear();
    m_offscreen.bindingGroups.clear();
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

void VulkanNBufferingSample::init()
{
    createDriver();
    getPhysicalDevices();
    createSurface();
    createDevice();
    createSwapchain();

    createCommandBuffer();
    createQueue();

    createDepthStencilTexture();
    createDepthStencilTextureView();

    createOffscreenColorMapTexture();
    createOffscreenColorMapTextureView();
    createOffscreenNormalMapTexture();
    createOffscreenNormalMapTextureView();
    createOffscreenCamera();
    createOffscreenUniformBuffer();
    createOffscreenVertexBuffer();

    createOffscreenPositionColorAttachmentTexture();
    createOffscreenPositionColorAttachmentTextureView();
    createOffscreenNormalColorAttachmentTexture();
    createOffscreenNormalColorAttachmentTextureView();
    createOffscreenAlbedoColorAttachmentTexture();
    createOffscreenAlbedoColorAttachmentTextureView();
    createOffscreenBindingGroupLayout();
    createOffscreenBindingGroup();
    createOffscreenPipelineLayout();
    createOffscreenPipeline();

    createCompositionUniformBuffer();
    createCompositionVertexBuffer();

    createCompositionBindingGroupLayout();
    createCompositionBindingGroup();
    createCompositionPipelineLayout();
    createCompositionPipeline();

    initImGui(m_device.get(), m_queue.get(), m_swapchain.get());

    m_initialized = true;
}

void VulkanNBufferingSample::update()
{
    updateOffscreenUniformBuffer();
    updateCompositionUniformBuffer();

    updateImGui();
    buildImGui();
}

void VulkanNBufferingSample::updateOffscreenUniformBuffer()
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

void VulkanNBufferingSample::updateCompositionUniformBuffer()
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
    uint32_t lightCountByteSize = static_cast<uint32_t>(sizeof(int));
    uint32_t showTextureByteSize = static_cast<uint32_t>(sizeof(int));
    uint32_t cameraPositionSize = sizeof(glm::vec3);

    auto& uniformBuffer = m_composition.uniformBuffer;
    void* pointer = uniformBuffer->map();
    char* bytePointer = static_cast<char*>(pointer);

    memcpy(bytePointer, m_composition.ubo.lights.data(), lightSize);
    bytePointer += lightSize;

    memcpy(bytePointer, &m_composition.ubo.cameraPosition, cameraPositionSize);
    bytePointer += cameraPositionSize;

    memcpy(bytePointer, &m_composition.ubo.lightCount, lightCountByteSize);
    bytePointer += lightCountByteSize;

    memcpy(bytePointer, &m_composition.ubo.showTexture, showTextureByteSize);
}

void VulkanNBufferingSample::updateImGui()
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
        if (ImGui::RadioButton("FIFO", m_presentMode == VK_PRESENT_MODE_FIFO_KHR))
        {
            m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
            recreateSwapchain();
        }
        else if (ImGui::RadioButton("FIFO RELAXED", m_presentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR))
        {
            m_presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            recreateSwapchain();
        }
        else if (ImGui::RadioButton("MAILBOX", m_presentMode == VK_PRESENT_MODE_MAILBOX_KHR))
        {
            m_presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            recreateSwapchain();
        }

        ImGui::Separator();

        if (ImGui::RadioButton("Min Count 2", m_minImageCount == 2))
        {
            m_minImageCount = 2;
            recreateSwapchain();
        }
        else if (ImGui::RadioButton("Min Count 3", m_minImageCount == 3))
        {
            m_minImageCount = 3;
            recreateSwapchain();
        }
        ImGui::End();
    }

    debugWindow();
    ImGui::Render();
}

void VulkanNBufferingSample::draw()
{
    CommandEncoderDescriptor commandEncoderDescriptor{};
    auto commandEncoder = m_commandBuffer->createCommandEncoder(commandEncoderDescriptor);

    auto renderView = m_swapchain->acquireNextTexture();

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
        depthStencilAttachment.textureView = m_depthStencilTextureView.get();
        depthStencilAttachment.depthLoadOp = LoadOp::kClear;
        depthStencilAttachment.depthStoreOp = StoreOp::kStore;
        depthStencilAttachment.clearValue = { .depth = 1.0f, .stencil = 0 };

        RenderPassEncoderDescriptor renderPassDescriptor{};
        renderPassDescriptor.colorAttachments = { positionColorAttachment, normalColorAttachment, albedoColorAttachment };
        renderPassDescriptor.depthStencilAttachment = depthStencilAttachment;
        renderPassDescriptor.sampleCount = m_sampleCount;

        auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(m_offscreen.renderPipeline.get());
        renderPassEncoder->setVertexBuffer(0, m_offscreen.vertexBuffer.get());
        renderPassEncoder->setIndexBuffer(m_offscreen.indexBuffer.get(), IndexFormat::kUint16);
        renderPassEncoder->setBindingGroup(0, m_offscreen.bindingGroups[0].get());
        renderPassEncoder->setBindingGroup(1, m_offscreen.bindingGroups[1].get());
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_offscreen.polygon.indices.size()), 1, 0, 0, 0);
        renderPassEncoder->end();
    }

    {
        ColorAttachment colorAttachment{};
        colorAttachment.loadOp = LoadOp::kClear;
        colorAttachment.storeOp = StoreOp::kStore;
        colorAttachment.renderView = renderView;
        colorAttachment.resolveView = nullptr;
        colorAttachment.clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };

        DepthStencilAttachment depthStencilAttachment{};
        depthStencilAttachment.textureView = m_depthStencilTextureView.get();
        depthStencilAttachment.depthLoadOp = LoadOp::kClear;
        depthStencilAttachment.depthStoreOp = StoreOp::kStore;
        depthStencilAttachment.clearValue = { .depth = 1.0f, .stencil = 0 };

        RenderPassEncoderDescriptor renderPassDescriptor{};
        renderPassDescriptor.colorAttachments = { colorAttachment };
        renderPassDescriptor.depthStencilAttachment = depthStencilAttachment;
        renderPassDescriptor.sampleCount = m_sampleCount;

        auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(m_composition.renderPipeline.get());
        renderPassEncoder->setVertexBuffer(0, m_composition.vertexBuffer.get());
        renderPassEncoder->setBindingGroup(0, m_composition.bindingGroups[0].get());
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->draw(static_cast<uint32_t>(m_composition.vertices.size()));
        renderPassEncoder->end();
    }

    drawImGui(commandEncoder.get(), renderView);

    m_queue->submit({ commandEncoder->finish() }, m_swapchain.get());
}

void VulkanNBufferingSample::createDriver()
{
    DriverDescriptor descriptor;
    descriptor.type = DriverType::kVulkan;
    m_driver = Driver::create(descriptor);
}

void VulkanNBufferingSample::getPhysicalDevices()
{
    m_physicalDevices = m_driver->getPhysicalDevices();
}

void VulkanNBufferingSample::createSurface()
{
    SurfaceDescriptor descriptor;
    descriptor.windowHandle = getWindowHandle();
    m_surface = m_driver->createSurface(descriptor);
}

void VulkanNBufferingSample::createSwapchain()
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

    auto vulkanDevice = downcast(m_device.get());
    VulkanSwapchainDescriptor vkdescriptor = generateVulkanSwapchainDescriptor(vulkanDevice, descriptor);

    vkdescriptor.presentMode = m_presentMode;
    vkdescriptor.minImageCount = m_minImageCount;
    vkdescriptor.oldSwapchain = m_swapchain != nullptr ? downcast(m_swapchain.get())->getVkSwapchainKHR() : VK_NULL_HANDLE;

    auto swapchain = vulkanDevice->createSwapchain(vkdescriptor);

    m_swapchain = std::move(swapchain);
}

void VulkanNBufferingSample::createDevice()
{
    // TODO: select suit device.
    PhysicalDevice* physicalDevice = m_physicalDevices[0].get();

    DeviceDescriptor descriptor;
    m_device = physicalDevice->createDevice(descriptor);
}

void VulkanNBufferingSample::createOffscreenPositionColorAttachmentTexture()
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

void VulkanNBufferingSample::createOffscreenPositionColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.positionColorAttachmentTextureView = m_offscreen.positionColorAttachmentTexture->createTextureView(descriptor);
}

void VulkanNBufferingSample::createOffscreenNormalColorAttachmentTexture()
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

void VulkanNBufferingSample::createOffscreenNormalColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.normalColorAttachmentTextureView = m_offscreen.normalColorAttachmentTexture->createTextureView(descriptor);
}

void VulkanNBufferingSample::createOffscreenAlbedoColorAttachmentTexture()
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

void VulkanNBufferingSample::createOffscreenAlbedoColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.albedoColorAttachmentTextureView = m_offscreen.albedoColorAttachmentTexture->createTextureView(descriptor);
}

void VulkanNBufferingSample::createOffscreenColorMapTexture()
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

void VulkanNBufferingSample::createOffscreenColorMapTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.colorMapTextureView = m_offscreen.colorMapTexture->createTextureView(descriptor);
}

void VulkanNBufferingSample::createOffscreenNormalMapTexture()
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

void VulkanNBufferingSample::createOffscreenNormalMapTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.normalMapTextureView = m_offscreen.normalMapTexture->createTextureView(descriptor);
}

void VulkanNBufferingSample::createOffscreenCamera()
{
    m_offscreen.camera = std::make_unique<PerspectiveCamera>(glm::radians(45.0f),
                                                             m_swapchain->getWidth() / static_cast<float>(m_swapchain->getHeight()),
                                                             0.1f,
                                                             1000.0f);
    m_offscreen.camera->lookAt(glm::vec3(0.0f, 0.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

void VulkanNBufferingSample::createOffscreenUniformBuffer()
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

void VulkanNBufferingSample::createOffscreenVertexBuffer()
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

void VulkanNBufferingSample::createOffscreenBindingGroupLayout()
{
    m_offscreen.bindingGroupLayouts.resize(2);

    {
        BufferBindingLayout bufferBindingLayout{};
        bufferBindingLayout.type = BufferBindingType::kUniform;
        bufferBindingLayout.index = 0;
        bufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;

        BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
        bindingGroupLayoutDescriptor.buffers = { bufferBindingLayout };

        m_offscreen.bindingGroupLayouts[0] = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
    }

    {
        SamplerBindingLayout colorSamplerBindingLayout{};
        colorSamplerBindingLayout.index = 0;
        colorSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

        SamplerBindingLayout normalSamplerBindingLayout{};
        normalSamplerBindingLayout.index = 1;
        normalSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

        TextureBindingLayout colorTextureBindingLayout{};
        colorTextureBindingLayout.index = 2;
        colorTextureBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

        TextureBindingLayout normalTextureBindingLayout{};
        normalTextureBindingLayout.index = 3;
        normalTextureBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

        BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
        bindingGroupLayoutDescriptor.samplers = { colorSamplerBindingLayout, normalSamplerBindingLayout };
        bindingGroupLayoutDescriptor.textures = { colorTextureBindingLayout, normalTextureBindingLayout };

        m_offscreen.bindingGroupLayouts[1] = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
    }
}

void VulkanNBufferingSample::createOffscreenBindingGroup()
{
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

    m_offscreen.bindingGroups.resize(2);
    {
        BufferBinding bufferBinding{};
        bufferBinding.buffer = m_offscreen.uniformBuffer.get();
        bufferBinding.index = 0;
        bufferBinding.offset = 0;
        bufferBinding.size = sizeof(MVP);

        BindingGroupDescriptor bindingGroupDescriptor{};
        bindingGroupDescriptor.buffers = { bufferBinding };
        bindingGroupDescriptor.layout = m_offscreen.bindingGroupLayouts[0].get();

        m_offscreen.bindingGroups[0] = m_device->createBindingGroup(bindingGroupDescriptor);
    }

    {
        SamplerBinding colorSamplerBinding{};
        colorSamplerBinding.index = 0;
        colorSamplerBinding.sampler = m_offscreen.colorMapSampler.get();

        SamplerBinding normalSamplerBinding{};
        normalSamplerBinding.index = 1;
        normalSamplerBinding.sampler = m_offscreen.normalMapSampler.get();

        TextureBinding colorTextureBinding{};
        colorTextureBinding.index = 2;
        colorTextureBinding.textureView = m_offscreen.colorMapTextureView.get();

        TextureBinding normalTextureBinding{};
        normalTextureBinding.index = 3;
        normalTextureBinding.textureView = m_offscreen.normalMapTextureView.get();

        BindingGroupDescriptor bindingGroupDescriptor{};
        bindingGroupDescriptor.samplers = { colorSamplerBinding, normalSamplerBinding };
        bindingGroupDescriptor.textures = { colorTextureBinding, normalTextureBinding };
        bindingGroupDescriptor.layout = m_offscreen.bindingGroupLayouts[1].get();

        m_offscreen.bindingGroups[1] = m_device->createBindingGroup(bindingGroupDescriptor);
    }
}

void VulkanNBufferingSample::createOffscreenPipelineLayout()
{
    PipelineLayoutDescriptor descriptor{};
    descriptor.layouts = { m_offscreen.bindingGroupLayouts[0].get(), m_offscreen.bindingGroupLayouts[1].get() };

    m_offscreen.pipelineLayout = m_device->createPipelineLayout(descriptor);
}

void VulkanNBufferingSample::createOffscreenPipeline()
{
    // Input Assembly
    InputAssemblyStage inputAssembly{};
    inputAssembly.topology = PrimitiveTopology::kTriangleList;

    // shader module
    {
        std::vector<char> vertexSource = utils::readFile(m_appDir / "offscreen.vert.spv", m_handle);

        ShaderModuleDescriptor shaderModuleDescriptor;
        shaderModuleDescriptor.code = vertexSource.data();
        shaderModuleDescriptor.codeSize = vertexSource.size();

        m_offscreen.vertexShaderModule = m_device->createShaderModule(shaderModuleDescriptor);
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

        vertexShage.shaderModule = m_offscreen.vertexShaderModule.get();
    }

    // Rasterization
    RasterizationStage rasterizationStage{};
    rasterizationStage.sampleCount = m_sampleCount;
    rasterizationStage.cullMode = CullMode::kNone;
    rasterizationStage.frontFace = FrontFace::kCounterClockwise;

    // shader module
    {
        std::vector<char> fragmentSource = utils::readFile(m_appDir / "offscreen.frag.spv", m_handle);

        ShaderModuleDescriptor shaderModuleDescriptor;
        shaderModuleDescriptor.code = fragmentSource.data();
        shaderModuleDescriptor.codeSize = fragmentSource.size();

        m_offscreen.fragmentShaderModule = m_device->createShaderModule(shaderModuleDescriptor);
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
        fragmentStage.shaderModule = m_offscreen.fragmentShaderModule.get();
    }
    DepthStencilStage depthStencil{};
    depthStencil.format = m_depthStencilTexture->getFormat();

    RenderPipelineDescriptor descriptor{};
    descriptor.layout = m_offscreen.pipelineLayout.get();
    descriptor.inputAssembly = inputAssembly;
    descriptor.vertex = vertexShage;
    descriptor.depthStencil = depthStencil;
    descriptor.rasterization = rasterizationStage;
    descriptor.fragment = fragmentStage;

    m_offscreen.renderPipeline = m_device->createRenderPipeline(descriptor);
}

void VulkanNBufferingSample::createCompositionBindingGroupLayout()
{
    m_composition.bindingGroupLayouts.resize(1);

    BindingGroupLayoutDescriptor descriptor{};

    SamplerBindingLayout positionSamplerBindingLayout{};
    positionSamplerBindingLayout.index = 0;
    positionSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

    SamplerBindingLayout normalSamplerBindingLayout{};
    normalSamplerBindingLayout.index = 1;
    normalSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

    SamplerBindingLayout albedoSamplerBindingLayout{};
    albedoSamplerBindingLayout.index = 2;
    albedoSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

    TextureBindingLayout positionTextureBindingLayout{};
    positionTextureBindingLayout.index = 3;
    positionTextureBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

    TextureBindingLayout normalTextureBindingLayout{};
    normalTextureBindingLayout.index = 4;
    normalTextureBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

    TextureBindingLayout albedoTextureBindingLayout{};
    albedoTextureBindingLayout.index = 5;
    albedoTextureBindingLayout.stages = BindingStageFlagBits::kFragmentStage;

    BufferBindingLayout uniformBufferBindingLayout{};
    uniformBufferBindingLayout.index = 6;
    uniformBufferBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
    uniformBufferBindingLayout.type = BufferBindingType::kUniform;

    descriptor.buffers = { uniformBufferBindingLayout };
    descriptor.samplers = { positionSamplerBindingLayout, normalSamplerBindingLayout, albedoSamplerBindingLayout };
    descriptor.textures = { positionTextureBindingLayout, normalTextureBindingLayout, albedoTextureBindingLayout };

    m_composition.bindingGroupLayouts[0] = m_device->createBindingGroupLayout(descriptor);
}

void VulkanNBufferingSample::createCompositionBindingGroup()
{
    m_composition.bindingGroups.resize(1);

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
    }

    TextureBinding positionTextureBinding{};
    positionTextureBinding.index = 3;
    positionTextureBinding.textureView = m_offscreen.positionColorAttachmentTextureView.get();

    TextureBinding normalTextureBinding{};
    normalTextureBinding.index = 4;
    normalTextureBinding.textureView = m_offscreen.normalColorAttachmentTextureView.get();

    TextureBinding albedoTextureBinding{};
    albedoTextureBinding.index = 5;
    albedoTextureBinding.textureView = m_offscreen.albedoColorAttachmentTextureView.get();

    BufferBinding uniformBufferBinding{};
    {
        uniformBufferBinding.buffer = m_composition.uniformBuffer.get();
        uniformBufferBinding.index = 6;
        uniformBufferBinding.offset = 0;
        uniformBufferBinding.size = m_composition.uniformBuffer->getSize();
    }

    BindingGroupDescriptor descriptor{};
    descriptor.layout = m_composition.bindingGroupLayouts[0].get();
    descriptor.buffers = { uniformBufferBinding };
    descriptor.samplers = { positionSamplerBinding, normalSamplerBinding, albedoSamplerBinding };
    descriptor.textures = { positionTextureBinding, normalTextureBinding, albedoTextureBinding };

    m_composition.bindingGroups[0] = m_device->createBindingGroup(descriptor);
}

void VulkanNBufferingSample::createCompositionPipelineLayout()
{
    PipelineLayoutDescriptor descriptor{};
    descriptor.layouts = { m_composition.bindingGroupLayouts[0].get() };

    m_composition.pipelineLayout = m_device->createPipelineLayout(descriptor);
}

void VulkanNBufferingSample::createCompositionPipeline()
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
    depthStencilStage.format = m_depthStencilTexture->getFormat();

    RenderPipelineDescriptor renderPipelineDescriptor{};
    renderPipelineDescriptor.inputAssembly = inputAssemblyStage;
    renderPipelineDescriptor.vertex = vertexStage;
    renderPipelineDescriptor.rasterization = rasterizationStage;
    renderPipelineDescriptor.fragment = fragmentStage;
    renderPipelineDescriptor.depthStencil = depthStencilStage;
    renderPipelineDescriptor.layout = m_composition.pipelineLayout.get();

    m_composition.renderPipeline = m_device->createRenderPipeline(renderPipelineDescriptor);
}

void VulkanNBufferingSample::createCompositionUniformBuffer()
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
    uint32_t lightCountByteSize = sizeof(int);
    uint32_t showTextureByteSize = sizeof(int);
    uint32_t cameraPositionSize = sizeof(glm::vec3);

    BufferDescriptor descriptor{};
    descriptor.size = lightSize + cameraPositionSize + lightCountByteSize + showTextureByteSize;
    descriptor.size += sizeof(int) * 2; // padding
    descriptor.usage = BufferUsageFlagBits::kUniform;

    m_composition.uniformBuffer = m_device->createBuffer(descriptor);

    auto& uniformBuffer = m_composition.uniformBuffer;
    void* pointer = uniformBuffer->map();
    char* bytePointer = static_cast<char*>(pointer);
    memcpy(bytePointer, m_composition.ubo.lights.data(), lightSize);
    bytePointer += lightSize;
    memcpy(bytePointer, &m_composition.ubo.cameraPosition, cameraPositionSize);
    bytePointer += cameraPositionSize;
    memcpy(bytePointer, &m_composition.ubo.lightCount, lightCountByteSize);
    bytePointer += lightCountByteSize;
    memcpy(bytePointer, &m_composition.ubo.showTexture, showTextureByteSize);
    // uniformBuffer->unmap();
}

void VulkanNBufferingSample::createCompositionVertexBuffer()
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

void VulkanNBufferingSample::createDepthStencilTexture()
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

    m_depthStencilTexture = m_device->createTexture(descriptor);
}

void VulkanNBufferingSample::createDepthStencilTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kDepth;

    m_depthStencilTextureView = m_depthStencilTexture->createTextureView(descriptor);
}

void VulkanNBufferingSample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    descriptor.usage = CommandBufferUsage::kOneTime;

    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void VulkanNBufferingSample::createQueue()
{
    QueueDescriptor descriptor{};
    descriptor.flags = QueueFlagBits::kGraphics;

    m_queue = m_device->createQueue(descriptor);
}

void VulkanNBufferingSample::recreateSwapchain()
{
    auto vulkanDevice = downcast(m_device.get());
    vulkanDevice->vkAPI.DeviceWaitIdle(vulkanDevice->getVkDevice());
    createSwapchain();
}

} // namespace jipu
