#include "vulkan_subpasses_sample.h"

#include <math.h>
#include <spdlog/spdlog.h>

#include <random>
#include <stdexcept>

#include "vulkan_binding_group.h"
#include "vulkan_binding_group_layout.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_command_encoder.h"
#include "vulkan_device.h"
#include "vulkan_driver.h"
#include "vulkan_framebuffer.h"
#include "vulkan_physical_device.h"
#include "vulkan_queue.h"
#include "vulkan_render_pass.h"
#include "vulkan_render_pass_encoder.h"
#include "vulkan_surface.h"
#include "vulkan_swapchain.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

namespace jipu
{

VulkanSubpassesSample::VulkanSubpassesSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
    // Do not call init function.
}

VulkanSubpassesSample::~VulkanSubpassesSample()
{
    clearImGui();

    m_depthStencilTextureView.reset();
    m_depthStencilTexture.reset();

    m_composition.vertexBuffer.reset();
    m_composition.uniformBuffer.reset();

    m_composition.subPasses.vertexShaderModule.reset();
    m_composition.subPasses.fragmentShaderModule.reset();
    m_composition.subPasses.renderPipeline.reset();
    m_composition.subPasses.pipelineLayout.reset();
    m_composition.subPasses.bindingGroupLayouts.clear();
    m_composition.subPasses.bindingGroups.clear();

    m_composition.renderPasses.vertexShaderModule.reset();
    m_composition.renderPasses.fragmentShaderModule.reset();
    m_composition.renderPasses.renderPipeline.reset();
    m_composition.renderPasses.pipelineLayout.reset();
    m_composition.renderPasses.bindingGroupLayouts.clear();
    m_composition.renderPasses.bindingGroups.clear();
    m_composition.renderPasses.albedoSampler.reset();
    m_composition.renderPasses.normalSampler.reset();
    m_composition.renderPasses.positionSampler.reset();

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

    m_offscreen.subPasses.vertexShaderModule.reset();
    m_offscreen.subPasses.fragmentShaderModule.reset();
    m_offscreen.subPasses.renderPipeline.reset();
    m_offscreen.subPasses.pipelineLayout.reset();
    m_offscreen.subPasses.bindingGroupLayouts.clear();
    m_offscreen.subPasses.bindingGroups.clear();
    m_offscreen.subPasses.albedoColorAttachmentTextureView.reset();
    m_offscreen.subPasses.albedoColorAttachmentTexture.reset();
    m_offscreen.subPasses.normalColorAttachmentTextureView.reset();
    m_offscreen.subPasses.normalColorAttachmentTexture.reset();
    m_offscreen.subPasses.positionColorAttachmentTextureView.reset();
    m_offscreen.subPasses.positionColorAttachmentTexture.reset();

    m_offscreen.renderPasses.vertexShaderModule.reset();
    m_offscreen.renderPasses.fragmentShaderModule.reset();
    m_offscreen.renderPasses.renderPipeline.reset();
    m_offscreen.renderPasses.pipelineLayout.reset();
    m_offscreen.renderPasses.bindingGroupLayouts.clear();
    m_offscreen.renderPasses.bindingGroups.clear();
    m_offscreen.renderPasses.albedoColorAttachmentTextureView.reset();
    m_offscreen.renderPasses.albedoColorAttachmentTexture.reset();
    m_offscreen.renderPasses.normalColorAttachmentTextureView.reset();
    m_offscreen.renderPasses.normalColorAttachmentTexture.reset();
    m_offscreen.renderPasses.positionColorAttachmentTextureView.reset();
    m_offscreen.renderPasses.positionColorAttachmentTexture.reset();

    m_queue.reset();
    m_commandBuffer.reset();

    m_swapchain.reset();
    m_device.reset();

    m_surface.reset();
    m_physicalDevices.clear();
    m_driver.reset();
}

void VulkanSubpassesSample::init()
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

void VulkanSubpassesSample::update()
{
    updateOffscreenUniformBuffer();
    updateCompositionUniformBuffer();

    updateImGui();
    buildImGui();
}

void VulkanSubpassesSample::updateOffscreenUniformBuffer()
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

void VulkanSubpassesSample::updateCompositionUniformBuffer()
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

void VulkanSubpassesSample::updateImGui()
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
        ImGui::Checkbox("Use Subpasses", &m_useSubpasses);
        ImGui::SliderInt("Number of Light", &m_composition.ubo.lightCount, 1, m_lightMax);
        // 라디오 버튼 1
        if (ImGui::RadioButton("Deferred", m_composition.ubo.showTexture == 0))
            m_composition.ubo.showTexture = 0;
        else if (ImGui::RadioButton("Position", m_composition.ubo.showTexture == 1))
            m_composition.ubo.showTexture = 1;
        else if (ImGui::RadioButton("Normal", m_composition.ubo.showTexture == 2))
            m_composition.ubo.showTexture = 2;
        else if (ImGui::RadioButton("Albedo", m_composition.ubo.showTexture == 3))
            m_composition.ubo.showTexture = 3;
        ImGui::End();
    }

    debugWindow();
    ImGui::Render();
}

void VulkanSubpassesSample::draw()
{
    CommandEncoderDescriptor commandEncoderDescriptor{};
    auto commandEncoder = m_commandBuffer->createCommandEncoder(commandEncoderDescriptor);
    auto vulkanDevice = downcast(m_device.get());
    auto vulkanCommandEncoder = downcast(commandEncoder.get());

    auto renderView = m_swapchain->acquireNextTexture();

    // render passes
    if (!m_useSubpasses)
    {
        {
            ColorAttachment positionColorAttachment{};
            positionColorAttachment.loadOp = LoadOp::kClear;
            positionColorAttachment.storeOp = StoreOp::kStore;
            positionColorAttachment.renderView = m_offscreen.renderPasses.positionColorAttachmentTextureView.get();
            positionColorAttachment.resolveView = nullptr;
            positionColorAttachment.clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };

            ColorAttachment normalColorAttachment{};
            normalColorAttachment.loadOp = LoadOp::kClear;
            normalColorAttachment.storeOp = StoreOp::kStore;
            normalColorAttachment.renderView = m_offscreen.renderPasses.normalColorAttachmentTextureView.get();
            normalColorAttachment.resolveView = nullptr;
            normalColorAttachment.clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };

            ColorAttachment albedoColorAttachment{};
            albedoColorAttachment.loadOp = LoadOp::kClear;
            albedoColorAttachment.storeOp = StoreOp::kStore;
            albedoColorAttachment.renderView = m_offscreen.renderPasses.albedoColorAttachmentTextureView.get();
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
            renderPassEncoder->setPipeline(m_offscreen.renderPasses.renderPipeline.get());
            renderPassEncoder->setVertexBuffer(0, m_offscreen.vertexBuffer.get());
            renderPassEncoder->setIndexBuffer(m_offscreen.indexBuffer.get(), IndexFormat::kUint16);
            renderPassEncoder->setBindingGroup(0, m_offscreen.renderPasses.bindingGroups[0].get());
            renderPassEncoder->setBindingGroup(1, m_offscreen.renderPasses.bindingGroups[1].get());
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
            renderPassEncoder->setPipeline(m_composition.renderPasses.renderPipeline.get());
            renderPassEncoder->setVertexBuffer(0, m_composition.vertexBuffer.get());
            renderPassEncoder->setBindingGroup(0, m_composition.renderPasses.bindingGroups[0].get());
            renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
            renderPassEncoder->setScissor(0, 0, m_width, m_height);
            renderPassEncoder->draw(static_cast<uint32_t>(m_composition.vertices.size()));
            renderPassEncoder->end();
        }
    }
    else
    {
        // subpasses
        auto vulkanRenderPass = getSubpassesRenderPass();
        auto vulkanFramebuffer = getSubpassesFrameBuffer(renderView);

        // first pass
        VulkanRenderPassEncoderDescriptor renderPassEncoderDescriptor{};
        renderPassEncoderDescriptor.renderPass = vulkanRenderPass->getVkRenderPass();
        renderPassEncoderDescriptor.framebuffer = vulkanFramebuffer->getVkFrameBuffer();
        renderPassEncoderDescriptor.renderArea.offset = { 0, 0 };
        renderPassEncoderDescriptor.renderArea.extent = { m_swapchain->getWidth(), m_swapchain->getHeight() };

        VkClearValue colorClearValue{};
        colorClearValue.color.float32[0] = 0.0f;
        colorClearValue.color.float32[1] = 0.0f;
        colorClearValue.color.float32[2] = 0.0f;
        colorClearValue.color.float32[3] = 0.0f;
        VkClearValue depthClearValue{ .depthStencil = { .depth = 1.0f, .stencil = 0 } };
        renderPassEncoderDescriptor.clearValues.push_back(colorClearValue);
        renderPassEncoderDescriptor.clearValues.push_back(colorClearValue);
        renderPassEncoderDescriptor.clearValues.push_back(colorClearValue);
        renderPassEncoderDescriptor.clearValues.push_back(colorClearValue);
        renderPassEncoderDescriptor.clearValues.push_back(depthClearValue);

        auto renderPassEncoder = vulkanCommandEncoder->beginRenderPass(renderPassEncoderDescriptor);
        VulkanRenderPassEncoder* vulkanRenderPassEncoder = downcast(renderPassEncoder.get());
        vulkanRenderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        vulkanRenderPassEncoder->setScissor(0, 0, m_width, m_height);

        // first pass
        vulkanRenderPassEncoder->setPipeline(m_offscreen.subPasses.renderPipeline.get());
        vulkanRenderPassEncoder->setVertexBuffer(0, m_offscreen.vertexBuffer.get());
        vulkanRenderPassEncoder->setIndexBuffer(m_offscreen.indexBuffer.get(), IndexFormat::kUint16);
        vulkanRenderPassEncoder->setBindingGroup(0, m_offscreen.subPasses.bindingGroups[0].get());
        vulkanRenderPassEncoder->setBindingGroup(1, m_offscreen.subPasses.bindingGroups[1].get());
        vulkanRenderPassEncoder->drawIndexed(static_cast<uint32_t>(m_offscreen.polygon.indices.size()), 1, 0, 0, 0);

        vulkanRenderPassEncoder->nextPass();

        // second pass
        vulkanRenderPassEncoder->setPipeline(m_composition.subPasses.renderPipeline.get());
        vulkanRenderPassEncoder->setVertexBuffer(0, m_composition.vertexBuffer.get());
        vulkanRenderPassEncoder->setBindingGroup(0, m_composition.subPasses.bindingGroups[0].get());
        vulkanRenderPassEncoder->setBindingGroup(1, m_composition.subPasses.bindingGroups[1].get());
        vulkanRenderPassEncoder->draw(static_cast<uint32_t>(m_composition.vertices.size()));

        vulkanRenderPassEncoder->end();
    }

    drawImGui(commandEncoder.get(), renderView);

    m_queue->submit({ commandEncoder->finish() }, m_swapchain.get());
}

void VulkanSubpassesSample::createDriver()
{
    DriverDescriptor descriptor;
    descriptor.type = DriverType::kVulkan;
    m_driver = Driver::create(descriptor);
}

void VulkanSubpassesSample::getPhysicalDevices()
{
    m_physicalDevices = m_driver->getPhysicalDevices();
}

void VulkanSubpassesSample::createSurface()
{
    SurfaceDescriptor descriptor;
    descriptor.windowHandle = getWindowHandle();
    m_surface = m_driver->createSurface(descriptor);
}

void VulkanSubpassesSample::createSwapchain()
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

void VulkanSubpassesSample::createDevice()
{
    // TODO: select suit device.
    PhysicalDevice* physicalDevice = m_physicalDevices[0].get();

    DeviceDescriptor descriptor;
    m_device = physicalDevice->createDevice(descriptor);
}

void VulkanSubpassesSample::createOffscreenPositionColorAttachmentTexture()
{
    // render passes
    {
        VulkanTextureDescriptor descriptor{};
        descriptor.imageType = VK_IMAGE_TYPE_2D;
        descriptor.format = VK_FORMAT_R16G16B16A16_UNORM;
        descriptor.extent = { m_swapchain->getWidth(), m_swapchain->getHeight(), 1 };
        descriptor.mipLevels = 1;
        descriptor.arrayLayers = 1;
        descriptor.samples = VK_SAMPLE_COUNT_1_BIT;
        descriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        descriptor.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        descriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
        descriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        m_offscreen.renderPasses.positionColorAttachmentTexture = downcast(m_device.get())->createTexture(descriptor);
    }

    // subpasses
    {
        VulkanTextureDescriptor descriptor{};
        descriptor.imageType = VK_IMAGE_TYPE_2D;
        descriptor.format = VK_FORMAT_R16G16B16A16_UNORM;
        descriptor.extent = { m_swapchain->getWidth(), m_swapchain->getHeight(), 1 };
        descriptor.mipLevels = 1;
        descriptor.arrayLayers = 1;
        descriptor.samples = VK_SAMPLE_COUNT_1_BIT;
        descriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        descriptor.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        descriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
        descriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        m_offscreen.subPasses.positionColorAttachmentTexture = downcast(m_device.get())->createTexture(descriptor);
    }
}

void VulkanSubpassesSample::createOffscreenPositionColorAttachmentTextureView()
{
    // render passes
    {
        TextureViewDescriptor descriptor{};
        descriptor.type = TextureViewType::k2D;
        descriptor.aspect = TextureAspectFlagBits::kColor;

        m_offscreen.renderPasses.positionColorAttachmentTextureView = m_offscreen.renderPasses.positionColorAttachmentTexture->createTextureView(descriptor);
    }

    // subpasses
    {
        TextureViewDescriptor descriptor{};
        descriptor.type = TextureViewType::k2D;
        descriptor.aspect = TextureAspectFlagBits::kColor;

        m_offscreen.subPasses.positionColorAttachmentTextureView = m_offscreen.subPasses.positionColorAttachmentTexture->createTextureView(descriptor);
    }
}

void VulkanSubpassesSample::createOffscreenNormalColorAttachmentTexture()
{
    // render passes
    {
        VulkanTextureDescriptor descriptor{};
        descriptor.imageType = VK_IMAGE_TYPE_2D;
        descriptor.format = VK_FORMAT_R16G16B16A16_UNORM;
        descriptor.extent = { m_swapchain->getWidth(), m_swapchain->getHeight(), 1 };
        descriptor.mipLevels = 1;
        descriptor.arrayLayers = 1;
        descriptor.samples = VK_SAMPLE_COUNT_1_BIT;
        descriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        descriptor.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        descriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
        descriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        m_offscreen.renderPasses.normalColorAttachmentTexture = downcast(m_device.get())->createTexture(descriptor);
    }

    // subpasses
    {
        VulkanTextureDescriptor descriptor{};
        descriptor.imageType = VK_IMAGE_TYPE_2D;
        descriptor.format = VK_FORMAT_R16G16B16A16_UNORM;
        descriptor.extent = { m_swapchain->getWidth(), m_swapchain->getHeight(), 1 };
        descriptor.mipLevels = 1;
        descriptor.arrayLayers = 1;
        descriptor.samples = VK_SAMPLE_COUNT_1_BIT;
        descriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        descriptor.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        descriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
        descriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        m_offscreen.subPasses.normalColorAttachmentTexture = downcast(m_device.get())->createTexture(descriptor);
    }
}

void VulkanSubpassesSample::createOffscreenNormalColorAttachmentTextureView()
{
    // render passe
    {
        TextureViewDescriptor descriptor{};
        descriptor.type = TextureViewType::k2D;
        descriptor.aspect = TextureAspectFlagBits::kColor;

        m_offscreen.renderPasses.normalColorAttachmentTextureView = m_offscreen.renderPasses.normalColorAttachmentTexture->createTextureView(descriptor);
    }

    // subpasses
    {
        TextureViewDescriptor descriptor{};
        descriptor.type = TextureViewType::k2D;
        descriptor.aspect = TextureAspectFlagBits::kColor;

        m_offscreen.subPasses.normalColorAttachmentTextureView = m_offscreen.subPasses.normalColorAttachmentTexture->createTextureView(descriptor);
    }
}

void VulkanSubpassesSample::createOffscreenAlbedoColorAttachmentTexture()
{
    // render passes
    {
        VulkanTextureDescriptor descriptor{};
        descriptor.imageType = VK_IMAGE_TYPE_2D;
        descriptor.format = VK_FORMAT_B8G8R8A8_UNORM;
        descriptor.extent = { m_swapchain->getWidth(), m_swapchain->getHeight(), 1 };
        descriptor.mipLevels = 1;
        descriptor.arrayLayers = 1;
        descriptor.samples = VK_SAMPLE_COUNT_1_BIT;
        descriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        descriptor.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        descriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
        descriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        m_offscreen.renderPasses.albedoColorAttachmentTexture = downcast(m_device.get())->createTexture(descriptor);
    }

    // subpasses
    {
        VulkanTextureDescriptor descriptor{};
        descriptor.imageType = VK_IMAGE_TYPE_2D;
        descriptor.format = VK_FORMAT_B8G8R8A8_UNORM;
        descriptor.extent = { m_swapchain->getWidth(), m_swapchain->getHeight(), 1 };
        descriptor.mipLevels = 1;
        descriptor.arrayLayers = 1;
        descriptor.samples = VK_SAMPLE_COUNT_1_BIT;
        descriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        descriptor.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        descriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
        descriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        m_offscreen.subPasses.albedoColorAttachmentTexture = downcast(m_device.get())->createTexture(descriptor);
    }
}

void VulkanSubpassesSample::createOffscreenAlbedoColorAttachmentTextureView()
{
    // render passes
    {
        TextureViewDescriptor descriptor{};
        descriptor.type = TextureViewType::k2D;
        descriptor.aspect = TextureAspectFlagBits::kColor;

        m_offscreen.renderPasses.albedoColorAttachmentTextureView = m_offscreen.renderPasses.albedoColorAttachmentTexture->createTextureView(descriptor);
    }

    // subpasses
    {
        TextureViewDescriptor descriptor{};
        descriptor.type = TextureViewType::k2D;
        descriptor.aspect = TextureAspectFlagBits::kColor;

        m_offscreen.subPasses.albedoColorAttachmentTextureView = m_offscreen.subPasses.albedoColorAttachmentTexture->createTextureView(descriptor);
    }
}

void VulkanSubpassesSample::createOffscreenColorMapTexture()
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

void VulkanSubpassesSample::createOffscreenColorMapTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.colorMapTextureView = m_offscreen.colorMapTexture->createTextureView(descriptor);
}

void VulkanSubpassesSample::createOffscreenNormalMapTexture()
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

void VulkanSubpassesSample::createOffscreenNormalMapTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_offscreen.normalMapTextureView = m_offscreen.normalMapTexture->createTextureView(descriptor);
}

void VulkanSubpassesSample::createOffscreenCamera()
{
    m_offscreen.camera = std::make_unique<PerspectiveCamera>(glm::radians(45.0f),
                                                             m_swapchain->getWidth() / static_cast<float>(m_swapchain->getHeight()),
                                                             0.1f,
                                                             1000.0f);
    m_offscreen.camera->lookAt(glm::vec3(0.0f, 0.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

void VulkanSubpassesSample::createOffscreenUniformBuffer()
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

void VulkanSubpassesSample::createOffscreenVertexBuffer()
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

void VulkanSubpassesSample::createOffscreenBindingGroupLayout()
{
    // render passes
    {
        m_offscreen.renderPasses.bindingGroupLayouts.resize(2);

        {
            BufferBindingLayout bufferBindingLayout{};
            bufferBindingLayout.type = BufferBindingType::kUniform;
            bufferBindingLayout.index = 0;
            bufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;

            BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
            bindingGroupLayoutDescriptor.buffers = { bufferBindingLayout };

            m_offscreen.renderPasses.bindingGroupLayouts[0] = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
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

            m_offscreen.renderPasses.bindingGroupLayouts[1] = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
        }
    }

    // subpasses
    {
        m_offscreen.subPasses.bindingGroupLayouts.resize(2);

        {
            BufferBindingLayout bufferBindingLayout{};
            bufferBindingLayout.type = BufferBindingType::kUniform;
            bufferBindingLayout.index = 0;
            bufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;

            BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
            bindingGroupLayoutDescriptor.buffers = { bufferBindingLayout };

            m_offscreen.subPasses.bindingGroupLayouts[0] = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
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

            m_offscreen.subPasses.bindingGroupLayouts[1] = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
        }
    }
}

void VulkanSubpassesSample::createOffscreenBindingGroup()
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

    // render passes
    {
        m_offscreen.renderPasses.bindingGroups.resize(2);
        {
            BufferBinding bufferBinding{};
            bufferBinding.buffer = m_offscreen.uniformBuffer.get();
            bufferBinding.index = 0;
            bufferBinding.offset = 0;
            bufferBinding.size = sizeof(MVP);

            BindingGroupDescriptor bindingGroupDescriptor{};
            bindingGroupDescriptor.buffers = { bufferBinding };
            bindingGroupDescriptor.layout = m_offscreen.renderPasses.bindingGroupLayouts[0].get();

            m_offscreen.renderPasses.bindingGroups[0] = m_device->createBindingGroup(bindingGroupDescriptor);
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
            bindingGroupDescriptor.layout = m_offscreen.renderPasses.bindingGroupLayouts[1].get();

            m_offscreen.renderPasses.bindingGroups[1] = m_device->createBindingGroup(bindingGroupDescriptor);
        }
    }

    // subpasses
    {
        m_offscreen.subPasses.bindingGroups.resize(2);
        {
            BufferBinding bufferBinding{};
            bufferBinding.buffer = m_offscreen.uniformBuffer.get();
            bufferBinding.index = 0;
            bufferBinding.offset = 0;
            bufferBinding.size = sizeof(MVP);

            BindingGroupDescriptor bindingGroupDescriptor{};
            bindingGroupDescriptor.buffers = { bufferBinding };
            bindingGroupDescriptor.layout = m_offscreen.subPasses.bindingGroupLayouts[0].get();

            m_offscreen.subPasses.bindingGroups[0] = m_device->createBindingGroup(bindingGroupDescriptor);
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
            bindingGroupDescriptor.layout = m_offscreen.subPasses.bindingGroupLayouts[1].get();

            m_offscreen.subPasses.bindingGroups[1] = m_device->createBindingGroup(bindingGroupDescriptor);
        }
    }
}

void VulkanSubpassesSample::createOffscreenPipelineLayout()
{
    // render passes
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_offscreen.renderPasses.bindingGroupLayouts[0].get(), m_offscreen.renderPasses.bindingGroupLayouts[1].get() };

        auto vulkanDevice = downcast(m_device.get());
        m_offscreen.renderPasses.pipelineLayout = vulkanDevice->createPipelineLayout(descriptor);
    }

    // subpasses
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_offscreen.subPasses.bindingGroupLayouts[0].get(), m_offscreen.subPasses.bindingGroupLayouts[1].get() };

        auto vulkanDevice = downcast(m_device.get());
        m_offscreen.subPasses.pipelineLayout = vulkanDevice->createPipelineLayout(descriptor);
    }
}

void VulkanSubpassesSample::createOffscreenPipeline()
{
    // render passes
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

            m_offscreen.renderPasses.vertexShaderModule = m_device->createShaderModule(shaderModuleDescriptor);
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

            vertexShage.shaderModule = m_offscreen.renderPasses.vertexShaderModule.get();
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

            m_offscreen.renderPasses.fragmentShaderModule = m_device->createShaderModule(shaderModuleDescriptor);
        }

        // Fragment Shader
        FragmentStage fragmentStage{};
        {
            // entry point
            fragmentStage.entryPoint = "main";

            // targets
            FragmentStage::Target positionTarget{};
            positionTarget.format = m_offscreen.renderPasses.positionColorAttachmentTexture->getFormat();

            FragmentStage::Target normalTarget{};
            normalTarget.format = m_offscreen.renderPasses.normalColorAttachmentTexture->getFormat();

            FragmentStage::Target albedoTarget{};
            albedoTarget.format = m_offscreen.renderPasses.albedoColorAttachmentTexture->getFormat();

            fragmentStage.targets = { positionTarget, normalTarget, albedoTarget };
            fragmentStage.shaderModule = m_offscreen.renderPasses.fragmentShaderModule.get();
        }
        DepthStencilStage depthStencil{};
        depthStencil.format = m_depthStencilTexture->getFormat();

        RenderPipelineDescriptor descriptor{};
        descriptor.layout = m_offscreen.renderPasses.pipelineLayout.get();
        descriptor.inputAssembly = inputAssembly;
        descriptor.vertex = vertexShage;
        descriptor.depthStencil = depthStencil;
        descriptor.rasterization = rasterizationStage;
        descriptor.fragment = fragmentStage;

        auto vulkanDevice = downcast(m_device.get());
        m_offscreen.renderPasses.renderPipeline = vulkanDevice->createRenderPipeline(descriptor);
    }

    // subpasses
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

            m_offscreen.subPasses.vertexShaderModule = m_device->createShaderModule(shaderModuleDescriptor);
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

            vertexShage.shaderModule = m_offscreen.subPasses.vertexShaderModule.get();
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

            m_offscreen.subPasses.fragmentShaderModule = m_device->createShaderModule(shaderModuleDescriptor);
        }

        // Fragment Shader
        FragmentStage fragmentStage{};
        {
            // entry point
            fragmentStage.entryPoint = "main";

            // targets
            FragmentStage::Target positionTarget{};
            positionTarget.format = m_offscreen.subPasses.positionColorAttachmentTexture->getFormat();

            FragmentStage::Target normalTarget{};
            normalTarget.format = m_offscreen.subPasses.normalColorAttachmentTexture->getFormat();

            FragmentStage::Target albedoTarget{};
            albedoTarget.format = m_offscreen.subPasses.albedoColorAttachmentTexture->getFormat();

            fragmentStage.targets = { positionTarget, normalTarget, albedoTarget };
            fragmentStage.shaderModule = m_offscreen.subPasses.fragmentShaderModule.get();
        }

        DepthStencilStage depthStencil{};
        depthStencil.format = m_depthStencilTexture->getFormat();

        RenderPipelineDescriptor descriptor{};
        descriptor.layout = m_offscreen.subPasses.pipelineLayout.get();
        descriptor.inputAssembly = inputAssembly;
        descriptor.vertex = vertexShage;
        descriptor.depthStencil = depthStencil;
        descriptor.rasterization = rasterizationStage;
        descriptor.fragment = fragmentStage;

        VulkanRenderPipelineDescriptor vkdescriptor{};
        vkdescriptor.inputAssemblyState = generateInputAssemblyStateCreateInfo(descriptor);
        vkdescriptor.vertexInputState = generateVertexInputStateCreateInfo(descriptor);
        vkdescriptor.viewportState = generateViewportStateCreateInfo(descriptor);
        vkdescriptor.rasterizationState = generateRasterizationStateCreateInfo(descriptor);
        vkdescriptor.multisampleState = generateMultisampleStateCreateInfo(descriptor);
        vkdescriptor.colorBlendState = generateColorBlendStateCreateInfo(descriptor);
        vkdescriptor.depthStencilState = generateDepthStencilStateCreateInfo(descriptor);
        vkdescriptor.dynamicState = generateDynamicStateCreateInfo(descriptor);
        vkdescriptor.stages = generateShaderStageCreateInfo(descriptor);

        vkdescriptor.layout = downcast(descriptor.layout);
        vkdescriptor.renderPass = getSubpassesRenderPass();
        // vkdescriptor.renderPass = getSubpassesCompatibleRenderPass();
        vkdescriptor.subpass = 0;
        vkdescriptor.basePipelineHandle = VK_NULL_HANDLE;
        vkdescriptor.basePipelineIndex = -1;

        auto vulkanDevice = downcast(m_device.get());
        m_offscreen.subPasses.renderPipeline = vulkanDevice->createRenderPipeline(vkdescriptor);
    }
}

void VulkanSubpassesSample::createCompositionBindingGroupLayout()
{
    // render passes
    {
        m_composition.renderPasses.bindingGroupLayouts.resize(1);

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

        m_composition.renderPasses.bindingGroupLayouts[0] = m_device->createBindingGroupLayout(descriptor);
    }

    // subpasses
    {
        m_composition.subPasses.bindingGroupLayouts.resize(2);

        {
            VkDescriptorSetLayoutBinding uniformBufferBindingLayout{};
            uniformBufferBindingLayout.binding = 0;
            uniformBufferBindingLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            uniformBufferBindingLayout.descriptorCount = 1;
            uniformBufferBindingLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            VulkanBindingGroupLayoutDescriptor descriptor{};
            descriptor.buffers = { uniformBufferBindingLayout };

            m_composition.subPasses.bindingGroupLayouts[0] = downcast(m_device.get())->createBindingGroupLayout(descriptor);
        }

        {
            VkDescriptorSetLayoutBinding positionTextureBindingLayout{};
            positionTextureBindingLayout.binding = 0;
            positionTextureBindingLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            positionTextureBindingLayout.descriptorCount = 1;
            positionTextureBindingLayout.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

            VkDescriptorSetLayoutBinding normalTextureBindingLayout = positionTextureBindingLayout;
            normalTextureBindingLayout.binding = 1;

            VkDescriptorSetLayoutBinding albedoTextureBindingLayout = positionTextureBindingLayout;
            albedoTextureBindingLayout.binding = 2;

            VulkanBindingGroupLayoutDescriptor descriptor{};
            descriptor.textures = { positionTextureBindingLayout, normalTextureBindingLayout, albedoTextureBindingLayout };

            m_composition.subPasses.bindingGroupLayouts[1] = downcast(m_device.get())->createBindingGroupLayout(descriptor);
        }
    }
}

void VulkanSubpassesSample::createCompositionBindingGroup()
{
    // render passes
    {
        m_composition.renderPasses.bindingGroups.resize(1);

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
            samplerDescriptor.lodMax = static_cast<float>(m_offscreen.renderPasses.positionColorAttachmentTexture->getMipLevels());

            m_composition.renderPasses.positionSampler = m_device->createSampler(samplerDescriptor);

            positionSamplerBinding.index = 0;
            positionSamplerBinding.sampler = m_composition.renderPasses.positionSampler.get();
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
            samplerDescriptor.lodMax = static_cast<float>(m_offscreen.renderPasses.normalColorAttachmentTexture->getMipLevels());

            m_composition.renderPasses.normalSampler = m_device->createSampler(samplerDescriptor);

            normalSamplerBinding.index = 1;
            normalSamplerBinding.sampler = m_composition.renderPasses.normalSampler.get();
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
            samplerDescriptor.lodMax = static_cast<float>(m_offscreen.renderPasses.albedoColorAttachmentTexture->getMipLevels());

            m_composition.renderPasses.albedoSampler = m_device->createSampler(samplerDescriptor);

            albedoSamplerBinding.index = 2;
            albedoSamplerBinding.sampler = m_composition.renderPasses.albedoSampler.get();
        }

        TextureBinding positionTextureBinding{};
        positionTextureBinding.index = 3;
        positionTextureBinding.textureView = m_offscreen.renderPasses.positionColorAttachmentTextureView.get();

        TextureBinding normalTextureBinding{};
        normalTextureBinding.index = 4;
        normalTextureBinding.textureView = m_offscreen.renderPasses.normalColorAttachmentTextureView.get();

        TextureBinding albedoTextureBinding{};
        albedoTextureBinding.index = 5;
        albedoTextureBinding.textureView = m_offscreen.renderPasses.albedoColorAttachmentTextureView.get();

        BufferBinding uniformBufferBinding{};
        {
            uniformBufferBinding.buffer = m_composition.uniformBuffer.get();
            uniformBufferBinding.index = 6;
            uniformBufferBinding.offset = 0;
            uniformBufferBinding.size = m_composition.uniformBuffer->getSize();
        }

        BindingGroupDescriptor descriptor{};
        descriptor.layout = m_composition.renderPasses.bindingGroupLayouts[0].get();
        descriptor.buffers = { uniformBufferBinding };
        descriptor.samplers = { positionSamplerBinding, normalSamplerBinding, albedoSamplerBinding };
        descriptor.textures = { positionTextureBinding, normalTextureBinding, albedoTextureBinding };

        m_composition.renderPasses.bindingGroups[0] = m_device->createBindingGroup(descriptor);
    }

    // subpasses
    {
        m_composition.subPasses.bindingGroups.resize(2);

        {
            BufferBinding uniformBufferBinding{};
            uniformBufferBinding.buffer = m_composition.uniformBuffer.get();
            uniformBufferBinding.index = 0;
            uniformBufferBinding.offset = 0;
            uniformBufferBinding.size = m_composition.uniformBuffer->getSize();

            BindingGroupDescriptor descriptor{};
            descriptor.layout = m_composition.subPasses.bindingGroupLayouts[0].get();
            descriptor.buffers = { uniformBufferBinding };

            m_composition.subPasses.bindingGroups[0] = m_device->createBindingGroup(descriptor);
        }

        {

            TextureBinding positionTextureBinding{};
            positionTextureBinding.index = 0;
            positionTextureBinding.textureView = m_offscreen.subPasses.positionColorAttachmentTextureView.get();

            TextureBinding normalTextureBinding{};
            normalTextureBinding.index = 1;
            normalTextureBinding.textureView = m_offscreen.subPasses.normalColorAttachmentTextureView.get();

            TextureBinding albedoTextureBinding{};
            albedoTextureBinding.index = 2;
            albedoTextureBinding.textureView = m_offscreen.subPasses.albedoColorAttachmentTextureView.get();

            BindingGroupDescriptor descriptor{};
            descriptor.layout = m_composition.subPasses.bindingGroupLayouts[1].get();
            descriptor.textures = { positionTextureBinding, normalTextureBinding, albedoTextureBinding };

            m_composition.subPasses.bindingGroups[1] = m_device->createBindingGroup(descriptor);
        }
    }
}

void VulkanSubpassesSample::createCompositionPipelineLayout()
{
    // render passes
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_composition.renderPasses.bindingGroupLayouts[0].get() };

        auto vulkanDevice = downcast(m_device.get());
        m_composition.renderPasses.pipelineLayout = vulkanDevice->createPipelineLayout(descriptor);
    }

    // subpasses
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_composition.subPasses.bindingGroupLayouts[0].get(), m_composition.subPasses.bindingGroupLayouts[1].get() };

        auto vulkanDevice = downcast(m_device.get());
        m_composition.subPasses.pipelineLayout = vulkanDevice->createPipelineLayout(descriptor);
    }
}

void VulkanSubpassesSample::createCompositionPipeline()
{
    // render passes
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
        renderPipelineDescriptor.layout = m_composition.renderPasses.pipelineLayout.get();

        m_composition.renderPasses.renderPipeline = m_device->createRenderPipeline(renderPipelineDescriptor);
    }

    // subpasses
    {
        // Input Assembly
        InputAssemblyStage inputAssemblyStage{};
        inputAssemblyStage.topology = PrimitiveTopology::kTriangleList;

        // Vertex
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
            std::vector<char> vertexSource = utils::readFile(m_appDir / "subpasses_composition.vert.spv", m_handle);

            ShaderModuleDescriptor shaderModuleDescriptor{};
            shaderModuleDescriptor.code = vertexSource.data();
            shaderModuleDescriptor.codeSize = static_cast<uint32_t>(vertexSource.size());
            m_composition.subPasses.vertexShaderModule = m_device->createShaderModule(shaderModuleDescriptor);

            vertexStage.shaderModule = m_composition.subPasses.vertexShaderModule.get();
        }

        // Rasterization
        RasterizationStage rasterizationStage{};
        rasterizationStage.cullMode = CullMode::kBack;
        rasterizationStage.frontFace = FrontFace::kCounterClockwise;
        rasterizationStage.sampleCount = m_sampleCount;

        // Fragment
        FragmentStage fragmentStage{};
        fragmentStage.entryPoint = "main";
        { // fragment shader targets
            FragmentStage::Target target{};
            target.format = m_swapchain->getTextureFormat();

            fragmentStage.targets = { target };
        }

        { // fragment shader module
            std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "subpasses_composition.frag.spv", m_handle);

            ShaderModuleDescriptor shaderModuleDescriptor{};
            shaderModuleDescriptor.code = fragmentShaderSource.data();
            shaderModuleDescriptor.codeSize = fragmentShaderSource.size();
            m_composition.subPasses.fragmentShaderModule = m_device->createShaderModule(shaderModuleDescriptor);

            fragmentStage.shaderModule = m_composition.subPasses.fragmentShaderModule.get();
        }

        // DepthStencil
        DepthStencilStage depthStencilStage{};
        depthStencilStage.format = m_depthStencilTexture->getFormat();

        RenderPipelineDescriptor descriptor{};
        descriptor.layout = m_composition.subPasses.pipelineLayout.get();
        descriptor.inputAssembly = inputAssemblyStage;
        descriptor.vertex = vertexStage;
        descriptor.depthStencil = depthStencilStage;
        descriptor.rasterization = rasterizationStage;
        descriptor.fragment = fragmentStage;

        VulkanRenderPipelineDescriptor vkdescriptor{};
        vkdescriptor.inputAssemblyState = generateInputAssemblyStateCreateInfo(descriptor);
        vkdescriptor.vertexInputState = generateVertexInputStateCreateInfo(descriptor);
        vkdescriptor.viewportState = generateViewportStateCreateInfo(descriptor);
        vkdescriptor.rasterizationState = generateRasterizationStateCreateInfo(descriptor);
        vkdescriptor.multisampleState = generateMultisampleStateCreateInfo(descriptor);
        vkdescriptor.colorBlendState = generateColorBlendStateCreateInfo(descriptor);
        vkdescriptor.depthStencilState = generateDepthStencilStateCreateInfo(descriptor);
        vkdescriptor.depthStencilState.depthWriteEnable = false;
        vkdescriptor.dynamicState = generateDynamicStateCreateInfo(descriptor);
        vkdescriptor.stages = generateShaderStageCreateInfo(descriptor);

        vkdescriptor.layout = downcast(descriptor.layout);
        vkdescriptor.renderPass = getSubpassesRenderPass();
        // vkdescriptor.renderPass = getSubpassesCompatibleRenderPass();
        vkdescriptor.subpass = 1;
        vkdescriptor.basePipelineHandle = VK_NULL_HANDLE;
        vkdescriptor.basePipelineIndex = -1;

        auto vulkanDevice = downcast(m_device.get());
        m_composition.subPasses.renderPipeline = vulkanDevice->createRenderPipeline(vkdescriptor);
    }
}

void VulkanSubpassesSample::createCompositionUniformBuffer()
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

void VulkanSubpassesSample::createCompositionVertexBuffer()
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

VulkanRenderPass* VulkanSubpassesSample::getSubpassesRenderPass()
{
    VulkanRenderPassDescriptor renderPassDescriptor{};

    // attachment descriptors
    renderPassDescriptor.attachmentDescriptions.resize(5);
    {
        // first pass
        {
            // position
            {
                auto texture = downcast(m_offscreen.subPasses.positionColorAttachmentTextureView.get())->getTexture();

                VkAttachmentDescription attachment{};
                attachment.format = ToVkFormat(texture->getFormat());
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

                renderPassDescriptor.attachmentDescriptions[0] = attachment;
            }

            // normal
            {
                auto texture = downcast(m_offscreen.subPasses.normalColorAttachmentTextureView.get())->getTexture();

                VkAttachmentDescription attachment{};
                attachment.format = ToVkFormat(texture->getFormat());
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

                renderPassDescriptor.attachmentDescriptions[1] = attachment;
            }

            // albedo
            {
                auto texture = downcast(m_offscreen.subPasses.albedoColorAttachmentTextureView.get())->getTexture();

                VkAttachmentDescription attachment{};
                attachment.format = ToVkFormat(texture->getFormat());
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

                renderPassDescriptor.attachmentDescriptions[2] = attachment;
            }
        }

        // second pass
        {
            auto swapchain = downcast(m_swapchain.get());

            VkAttachmentDescription attachment{};
            attachment.format = ToVkFormat(swapchain->getTextureFormat());
            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

            renderPassDescriptor.attachmentDescriptions[3] = attachment;
        }

        // depth
        {
            auto texture = downcast(m_depthStencilTextureView.get())->getTexture();

            VkAttachmentDescription attachment{};
            attachment.format = ToVkFormat(texture->getFormat());
            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

            renderPassDescriptor.attachmentDescriptions[4] = attachment;
        }
    }

    // subpass descriptions
    renderPassDescriptor.subpassDescriptions.resize(2);
    {
        // first pass
        {
            VulkanSubpassDescription description{};
            description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            // color attachment
            {
                description.colorAttachments.resize(3);
                // position attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 0;
                    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                    description.colorAttachments[0] = reference;
                }
                // normal attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 1;
                    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                    description.colorAttachments[1] = reference;
                }

                // albedo attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 2;
                    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                    description.colorAttachments[2] = reference;
                }
            }
            // depth attachment
            {
                VkAttachmentReference reference{};
                reference.attachment = 4;
                reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                description.depthStencilAttachment = reference;
            }

            renderPassDescriptor.subpassDescriptions[0] = description;
        }
        // second pass
        {
            VulkanSubpassDescription description{};
            description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            // input attachment
            {
                description.inputAttachments.resize(3);
                // position attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 0;
                    reference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    description.inputAttachments[0] = reference;
                }

                // normal attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 1;
                    reference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    description.inputAttachments[1] = reference;
                }

                // albedo attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 2;
                    reference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    description.inputAttachments[2] = reference;
                }
            }
            // color attachment
            {
                description.colorAttachments.resize(1);
                // render attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 3;
                    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                    description.colorAttachments[0] = reference;
                }
            }
            // depth attachment
            {
                VkAttachmentReference reference{};
                reference.attachment = 4;
                reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                description.depthStencilAttachment = reference;
            }

            renderPassDescriptor.subpassDescriptions[1] = description;
        }
    }

    // subpass dependencies
    {
        renderPassDescriptor.subpassDependencies.resize(3);
        {
            VkSubpassDependency subpassDependency{};
            subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependency.dstSubpass = 0;
            subpassDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            subpassDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            subpassDependency.srcAccessMask = 0;
            subpassDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            subpassDependency.dependencyFlags = 0;

            renderPassDescriptor.subpassDependencies[0] = subpassDependency;
        }
        {
            VkSubpassDependency subpassDependency{};
            subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependency.dstSubpass = 0;
            subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.srcAccessMask = 0;
            subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            subpassDependency.dependencyFlags = 0;

            renderPassDescriptor.subpassDependencies[1] = subpassDependency;
        }
        {
            VkSubpassDependency subpassDependency{};
            subpassDependency.srcSubpass = 0;
            subpassDependency.dstSubpass = 1;
            subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            subpassDependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
            subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            renderPassDescriptor.subpassDependencies[2] = subpassDependency;
        }
    }

    auto vulkanDevice = downcast(m_device.get());
    return vulkanDevice->getRenderPass(renderPassDescriptor);
}

VulkanRenderPass* VulkanSubpassesSample::getSubpassesCompatibleRenderPass()
{
    VulkanRenderPassDescriptor renderPassDescriptor{};

    // attachment descriptors
    renderPassDescriptor.attachmentDescriptions.resize(5);
    {
        // first pass
        {
            // position
            {
                auto texture = downcast(m_offscreen.subPasses.positionColorAttachmentTextureView.get())->getTexture();

                VkAttachmentDescription attachment{};
                attachment.format = ToVkFormat(texture->getFormat());
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

                renderPassDescriptor.attachmentDescriptions[0] = attachment;
            }

            // normal
            {
                auto texture = downcast(m_offscreen.subPasses.normalColorAttachmentTextureView.get())->getTexture();

                VkAttachmentDescription attachment{};
                attachment.format = ToVkFormat(texture->getFormat());
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

                renderPassDescriptor.attachmentDescriptions[1] = attachment;
            }

            // albedo
            {
                auto texture = downcast(m_offscreen.subPasses.albedoColorAttachmentTextureView.get())->getTexture();

                VkAttachmentDescription attachment{};
                attachment.format = ToVkFormat(texture->getFormat());
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

                renderPassDescriptor.attachmentDescriptions[2] = attachment;
            }
        }

        // second pass
        {
            auto swapchain = downcast(m_swapchain.get());

            VkAttachmentDescription attachment{};
            attachment.format = ToVkFormat(swapchain->getTextureFormat());
            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

            renderPassDescriptor.attachmentDescriptions[3] = attachment;
        }

        // depth
        {
            auto texture = downcast(m_depthStencilTextureView.get())->getTexture();

            VkAttachmentDescription attachment{};
            attachment.format = ToVkFormat(texture->getFormat());
            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.samples = ToVkSampleCountFlagBits(m_sampleCount);

            renderPassDescriptor.attachmentDescriptions[4] = attachment;
        }
    }

    // subpass descriptions
    renderPassDescriptor.subpassDescriptions.resize(2);
    {
        // first pass
        {
            VulkanSubpassDescription description{};
            description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            // color attachment
            {
                description.colorAttachments.resize(3);
                // position attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 0;
                    reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                    description.colorAttachments[0] = reference;
                }
                // normal attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 1;
                    reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                    description.colorAttachments[1] = reference;
                }

                // albedo attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 2;
                    reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                    description.colorAttachments[2] = reference;
                }
            }
            // depth attachment
            {
                VkAttachmentReference reference{};
                reference.attachment = 4;
                reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                description.depthStencilAttachment = reference;
            }

            renderPassDescriptor.subpassDescriptions[0] = description;
        }
        // second pass
        {
            VulkanSubpassDescription description{};
            description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            // color attachment
            {
                description.colorAttachments.resize(1);
                // render attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 3;
                    reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                    description.colorAttachments[0] = reference;
                }
            }
            // input attachment
            {
                description.inputAttachments.resize(3);
                // position attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 0;
                    reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                    description.inputAttachments[0] = reference;
                }

                // normal attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 1;
                    reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                    description.inputAttachments[1] = reference;
                }

                // albedo attachment
                {
                    VkAttachmentReference reference{};
                    reference.attachment = 2;
                    reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                    description.inputAttachments[2] = reference;
                }
            }
            // depth attachment
            {
                VkAttachmentReference reference{};
                reference.attachment = 4;
                reference.layout = VK_IMAGE_LAYOUT_GENERAL;

                description.depthStencilAttachment = reference;
            }

            renderPassDescriptor.subpassDescriptions[1] = description;
        }
    }

    // subpass dependencies
    {
        renderPassDescriptor.subpassDependencies.resize(4);
        {
            VkSubpassDependency subpassDependency{};
            subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependency.dstSubpass = 0;
            subpassDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            subpassDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            subpassDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            subpassDependency.dependencyFlags = 0;

            renderPassDescriptor.subpassDependencies[0] = subpassDependency;
        }
        {
            VkSubpassDependency subpassDependency{};
            subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependency.dstSubpass = 0;
            subpassDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.srcAccessMask = 0;
            subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            subpassDependency.dependencyFlags = 0;

            renderPassDescriptor.subpassDependencies[1] = subpassDependency;
        }
        {
            VkSubpassDependency subpassDependency{};
            subpassDependency.srcSubpass = 0;
            subpassDependency.dstSubpass = 1;
            subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            subpassDependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
            subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            renderPassDescriptor.subpassDependencies[2] = subpassDependency;
        }
        {
            VkSubpassDependency subpassDependency{};
            subpassDependency.srcSubpass = 0;
            subpassDependency.dstSubpass = 1;
            subpassDependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            subpassDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            subpassDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            renderPassDescriptor.subpassDependencies[3] = subpassDependency;
        }
    }

    auto vulkanDevice = downcast(m_device.get());
    return vulkanDevice->getRenderPass(renderPassDescriptor);
}

VulkanFramebuffer* VulkanSubpassesSample::getSubpassesFrameBuffer(TextureView* renderView)
{
    VulkanFramebufferDescriptor descriptor{};
    descriptor.renderPass = getSubpassesRenderPass()->getVkRenderPass();
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.layers = 1;

    // first pass
    descriptor.attachments.push_back(downcast(m_offscreen.subPasses.positionColorAttachmentTextureView.get())->getVkImageView());
    descriptor.attachments.push_back(downcast(m_offscreen.subPasses.normalColorAttachmentTextureView.get())->getVkImageView());
    descriptor.attachments.push_back(downcast(m_offscreen.subPasses.albedoColorAttachmentTextureView.get())->getVkImageView());

    // second pass
    descriptor.attachments.push_back(downcast(renderView)->getVkImageView());

    // depth
    descriptor.attachments.push_back(downcast(m_depthStencilTextureView.get())->getVkImageView());

    auto vulkanDevice = downcast(m_device.get());
    return vulkanDevice->getFrameBuffer(descriptor);
}

void VulkanSubpassesSample::createDepthStencilTexture()
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

void VulkanSubpassesSample::createDepthStencilTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kDepth;

    m_depthStencilTextureView = m_depthStencilTexture->createTextureView(descriptor);
}

void VulkanSubpassesSample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    descriptor.usage = CommandBufferUsage::kOneTime;

    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void VulkanSubpassesSample::createQueue()
{
    QueueDescriptor descriptor{};
    descriptor.flags = QueueFlagBits::kGraphics;

    m_queue = m_device->createQueue(descriptor);
}

} // namespace jipu