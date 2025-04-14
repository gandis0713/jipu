#include "june_vulkan_service3.h"

#include <spdlog/spdlog.h>

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <android/sync.h>
#endif

namespace jipu
{

JuneVulkanService3::JuneVulkanService3(const JuneServiceDescriptor& descriptor)
    : JuneVulkanService(descriptor)
{
}

JuneVulkanService3::~JuneVulkanService3()
{
}

void JuneVulkanService3::begin()
{
    JuneVulkanService::begin();

    // Create Shared Memory
    JuneSharedMemory juneSharedMemory{};
    {
        JuneSharedMemoryDescriptor juneSharedMemoryDescriptor{};
#if defined(__ANDROID__) || defined(ANDROID)
        AHardwareBuffer_Desc ahbDesc = {
            .width = m_descriptor.width,
            .height = m_descriptor.height,
            .layers = 1,
            .format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM,
            .usage = AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT
        };

        JuneSharedMemoryAHardwareBufferDescriptor juneSharedMemoryAHardwareBufferDescriptor{};
        juneSharedMemoryAHardwareBufferDescriptor.chain.sType = JuneSType_AHardwareBufferSharedMemory;
        juneSharedMemoryAHardwareBufferDescriptor.aHardwareBuffer = nullptr;
        juneSharedMemoryAHardwareBufferDescriptor.aHardwareBufferDesc = &ahbDesc;

        juneSharedMemoryDescriptor.nextInChain = &juneSharedMemoryAHardwareBufferDescriptor.chain;
#endif
        juneSharedMemory = m_juneAPI.InstanceCreateSharedMemory(m_juneInstance, &juneSharedMemoryDescriptor);
        m_sharingObjects.sharedMemory = juneSharedMemory;
    }

    // Create Resource and connect
    {
        {
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
#if defined(__ANDROID__) || defined(ANDROID)
            imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
#else
            imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
#endif
            imageInfo.extent.width = m_descriptor.width;
            imageInfo.extent.height = m_descriptor.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.tiling = VK_IMAGE_TILING_LINEAR; // VK_IMAGE_TILING_OPTIMAL is better for performance. but size is larger.
            imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            // imageInfo.flags = VK_IMAGE_CREATE_ALIAS_BIT;

            JuneResourceVkImageDescriptor juneResourceVkImageDescriptor{};
            juneResourceVkImageDescriptor.chain.sType = JuneSType_VkImageResourceDescriptor;
            juneResourceVkImageDescriptor.vkImageCreateInfo = &imageInfo;

            JuneResourceDescriptor juneResourceDescriptor{};
            juneResourceDescriptor.nextInChain = &juneResourceVkImageDescriptor.chain;
            juneResourceDescriptor.sharedMemory = juneSharedMemory;

            m_offscreen.resource = m_juneAPI.ApiContextCreateResource(m_juneApiContext, &juneResourceDescriptor);
        }
        {

            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
#if defined(__ANDROID__) || defined(ANDROID)
            imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
#else
            imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
#endif
            imageInfo.extent.width = m_descriptor.width;
            imageInfo.extent.height = m_descriptor.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.tiling = VK_IMAGE_TILING_LINEAR; // VK_IMAGE_TILING_OPTIMAL is better for performance. but size is larger.
            imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            // imageInfo.flags = VK_IMAGE_CREATE_ALIAS_BIT;

            JuneResourceVkImageDescriptor juneResourceVkImageDescriptor{};
            juneResourceVkImageDescriptor.chain.sType = JuneSType_VkImageResourceDescriptor;
            juneResourceVkImageDescriptor.vkImageCreateInfo = &imageInfo;

            JuneResourceDescriptor juneResourceDescriptor{};
            juneResourceDescriptor.nextInChain = &juneResourceVkImageDescriptor.chain;
            juneResourceDescriptor.sharedMemory = juneSharedMemory;

            m_onscreen.resource = m_juneAPI.ApiContextCreateResource(m_juneApiContext, &juneResourceDescriptor);
        }
    }

    // Create Resource
    createOffscreenImage();
    createOnscreenImage();

    createOffscreenTexture();
    createOffscreenTextureView();
    createOffscreenVertexBuffer();
    createOffscreenIndexBuffer();
    createOffscreenUniformBuffer();
    createOffscreenBindGroupLayout();
    createOffscreenBindGroup();
    createOffscreenRenderPipeline();

    createOnscreenTexture();
    createOnscreenTextureView();
    createOnscreenVertexBuffer();
    createOnscreenIndexBuffer();
    createOnscreenSampler();
    createOnscreenBindGroupLayout();
    createOnscreenBindGroup();
    createOnscreenRenderPipeline();

    createCamera();
}

void JuneVulkanService3::work()
{
    updateOffscreenUniformBuffer();

    auto offscreenRenderView = m_offscreen.renderTextureView.get();
    auto offscreenVulkanRenderTexture = static_cast<VulkanTexture*>(offscreenRenderView->getTexture());

    auto onscreenRenderTextureView = m_swapchain->acquireNextTextureView();
    auto onscreenVulkanRenderTexture = static_cast<VulkanTexture*>(onscreenRenderTextureView->getTexture());

    auto onscreenReadTextureView = m_onscreen.renderTextureView.get();
    auto onscreenReadVulkanRenderTexture = static_cast<VulkanTexture*>(onscreenReadTextureView->getTexture());

    bool offscreenImageLayoutTransitionBefore = true;
    bool offscreenImageLayoutTransitionAfter = true;
    bool onscreenImageLayoutTransitionBefore = true;
    bool onscreenImageLayoutTransitionAfter = true;
    // offscreen image layout transition
    if (offscreenImageLayoutTransitionBefore)
    {
        CommandEncoderDescriptor commandDescriptor{};
        auto commandEncoder = m_device->createCommandEncoder(commandDescriptor);
        auto vulkanCommandEncoder = static_cast<VulkanCommandEncoder*>(commandEncoder.get());

        VkImageSubresourceRange range;
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseMipLevel = 0;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.layerCount = 1;

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = VK_NULL_HANDLE;
        barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL; // or offscreenVulkanRenderTexture->getCurrentLayout(0);
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = offscreenVulkanRenderTexture->getVkImage();
        barrier.subresourceRange = range;

        VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        vulkanCommandEncoder->imageTransition(offscreenVulkanRenderTexture, barrier, srcStage, dstStage);
        auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
        m_queue->submit({ commandBuffer.get() });
    }

    // offscreen pass
    {
        ColorAttachment attachment{
            .renderView = offscreenRenderView
        };
        attachment.clearValue = { 0.0, 0.0, 0.0, 0.0 };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;

        RenderPassEncoderDescriptor renderPassDescriptor{
            .colorAttachments = { attachment }
        };

        CommandEncoderDescriptor commandDescriptor{};
        auto commandEncoder = m_device->createCommandEncoder(commandDescriptor);

        auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(m_offscreen.renderPipeline.get());
        renderPassEncoder->setBindGroup(0, m_offscreen.bindGroup.get());
        renderPassEncoder->setVertexBuffer(0, m_offscreen.vertexBuffer.get());
        renderPassEncoder->setIndexBuffer(m_offscreen.indexBuffer.get(), IndexFormat::kUint16);
        renderPassEncoder->setScissor(0, 0, m_descriptor.width, m_descriptor.height);
        renderPassEncoder->setViewport(0, 0, m_descriptor.width, m_descriptor.height, 0, 1);
        renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_offscreenIndices.size()), 1, 0, 0, 0);
        renderPassEncoder->end();

        auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
        m_queue->submit({ commandBuffer.get() });
    }

    // image layout transition
    {
        // offscreen
        if (offscreenImageLayoutTransitionAfter)
        {
            CommandEncoderDescriptor commandDescriptor{};
            auto commandEncoder = m_device->createCommandEncoder(commandDescriptor);
            auto vulkanCommandEncoder = static_cast<VulkanCommandEncoder*>(commandEncoder.get());

            VkImageSubresourceRange range;
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = VK_NULL_HANDLE;
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // or offscreenVulkanRenderTexture->getCurrentLayout(0);
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = offscreenVulkanRenderTexture->getVkImage();
            barrier.subresourceRange = range;

            VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

            vulkanCommandEncoder->imageTransition(offscreenVulkanRenderTexture, barrier, srcStage, dstStage);
            auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
            m_queue->submit({ commandBuffer.get() });
        }

        // onscreen
        if (onscreenImageLayoutTransitionBefore)
        {
            CommandEncoderDescriptor commandDescriptor{};
            auto commandEncoder = m_device->createCommandEncoder(commandDescriptor);
            auto vulkanCommandEncoder = static_cast<VulkanCommandEncoder*>(commandEncoder.get());

            VkImageSubresourceRange range;
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = VK_NULL_HANDLE;
            barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL; // or onscreenReadVulkanRenderTexture->getCurrentLayout(0);
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = onscreenReadVulkanRenderTexture->getVkImage();
            barrier.subresourceRange = range;

            VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

            vulkanCommandEncoder->imageTransition(onscreenReadVulkanRenderTexture, barrier, srcStage, dstStage);
            auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
            m_queue->submit({ commandBuffer.get() });
        }
    }

    // onscreen pass
    {
        ColorAttachment attachment{
            .renderView = onscreenRenderTextureView
        };
        attachment.clearValue = { 0.0, 0.0, 0.0, 0.0 };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;

        RenderPassEncoderDescriptor renderPassDescriptor{
            .colorAttachments = { attachment }
        };

        CommandEncoderDescriptor commandDescriptor{};
        auto commandEncoder = m_device->createCommandEncoder(commandDescriptor);

        auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(m_onscreen.renderPipeline.get());
        renderPassEncoder->setBindGroup(0, m_onscreen.bindGroup.get());
        renderPassEncoder->setVertexBuffer(0, m_onscreen.vertexBuffer.get());
        renderPassEncoder->setIndexBuffer(m_onscreen.indexBuffer.get(), IndexFormat::kUint16);
        renderPassEncoder->setScissor(0, 0, m_descriptor.width, m_descriptor.height);
        renderPassEncoder->setViewport(0, 0, m_descriptor.width, m_descriptor.height, 0, 1);
        renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_onscreenIndices.size()), 1, 0, 0, 0);
        renderPassEncoder->end();

        auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
        m_queue->submit({ commandBuffer.get() });
        m_swapchain->present();

        // onscreen image layout transition
        if (onscreenImageLayoutTransitionAfter)
        {
            CommandEncoderDescriptor commandDescriptor{};
            auto commandEncoder = m_device->createCommandEncoder(commandDescriptor);
            auto vulkanCommandEncoder = static_cast<VulkanCommandEncoder*>(commandEncoder.get());

            VkImageSubresourceRange range;
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = VK_NULL_HANDLE;
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // or onscreenReadVulkanRenderTexture->getCurrentLayout(0);
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = onscreenReadVulkanRenderTexture->getVkImage();
            barrier.subresourceRange = range;

            VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

            vulkanCommandEncoder->imageTransition(onscreenReadVulkanRenderTexture, barrier, srcStage, dstStage);
            auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
            m_queue->submit({ commandBuffer.get() });
        }
    }
}

JuneServiceShareObjects JuneVulkanService3::getSharingObject() const
{
    return m_sharingObjects;
}

void JuneVulkanService3::setSharedObjects(const JuneServiceShareObjects& sharedObjects)
{
    m_sharedObjects = sharedObjects;
}

void JuneVulkanService3::createOffscreenImage()
{
    JuneGetResourceDescriptor juneGetResourceDescriptor{};
    m_offscreen.image = reinterpret_cast<VkImage>(m_juneAPI.ResourceGetResource(m_offscreen.resource,
                                                                                &juneGetResourceDescriptor));
    assert(m_offscreen.image);
}

void JuneVulkanService3::createOffscreenTexture()
{
    VulkanTextureDescriptor vulkanTextureDescriptor{};
    vulkanTextureDescriptor.imageType = VK_IMAGE_TYPE_2D;
#if defined(__ANDROID__) || defined(ANDROID)
    vulkanTextureDescriptor.format = VK_FORMAT_R8G8B8A8_UNORM;
#else
    vulkanTextureDescriptor.format = VK_FORMAT_B8G8R8A8_UNORM;
#endif
    vulkanTextureDescriptor.extent.width = m_descriptor.width;
    vulkanTextureDescriptor.extent.height = m_descriptor.height;
    vulkanTextureDescriptor.extent.depth = 1;
    vulkanTextureDescriptor.mipLevels = 1;
    vulkanTextureDescriptor.arrayLayers = 1;
    vulkanTextureDescriptor.samples = VK_SAMPLE_COUNT_1_BIT;
    vulkanTextureDescriptor.tiling = VK_IMAGE_TILING_LINEAR; // VK_IMAGE_TILING_OPTIMAL is better for performance. but size is larger.
    vulkanTextureDescriptor.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    vulkanTextureDescriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vulkanTextureDescriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vulkanTextureDescriptor.owner = VulkanTextureOwner::kExternal;
    vulkanTextureDescriptor.image = m_offscreen.image;
    // vulkanTextureDescriptor.flags = VK_IMAGE_CREATE_ALIAS_BIT;

    auto vulkanDevice = static_cast<VulkanDevice*>(m_device.get());
    m_offscreen.renderTexture = vulkanDevice->createTexture(vulkanTextureDescriptor);

    // image layout transition
    {
        auto vulkanRenderTexture = static_cast<VulkanTexture*>(m_offscreen.renderTexture.get());

        CommandEncoderDescriptor commandDescriptor{};
        auto commandEncoder = m_device->createCommandEncoder(commandDescriptor);
        auto vulkanCommandEncoder = static_cast<VulkanCommandEncoder*>(commandEncoder.get());

        {
            VkImageSubresourceRange range;
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = VK_NULL_HANDLE;
            barrier.srcAccessMask = VK_ACCESS_NONE;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; // or vulkanRenderTexture->getCurrentLayout(0);
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = vulkanRenderTexture->getVkImage();
            barrier.subresourceRange = range;

            VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

            vulkanCommandEncoder->imageTransition(vulkanRenderTexture, barrier, srcStage, dstStage);
        }

        auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
        m_queue->submit({ commandBuffer.get() });
    }
}

void JuneVulkanService3::createOffscreenTextureView()
{
    TextureViewDescriptor textureViewDescriptor;
    textureViewDescriptor.aspect = TextureAspectFlagBits::kColor;
    textureViewDescriptor.dimension = TextureViewDimension::k2D;

    m_offscreen.renderTextureView = m_offscreen.renderTexture->createTextureView(textureViewDescriptor);
}

void JuneVulkanService3::createOffscreenVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_offscreenVertices.size() * sizeof(OffscreenVertex);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_offscreen.vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_offscreen.vertexBuffer->map();
    memcpy(pointer, m_offscreenVertices.data(), descriptor.size);
    m_offscreen.vertexBuffer->unmap();
}

void JuneVulkanService3::createOffscreenIndexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_offscreenIndices.size() * sizeof(uint16_t);
    descriptor.usage = BufferUsageFlagBits::kIndex;

    m_offscreen.indexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_offscreen.indexBuffer->map();
    memcpy(pointer, m_offscreenIndices.data(), descriptor.size);
    m_offscreen.indexBuffer->unmap();
}

void JuneVulkanService3::createOffscreenUniformBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = sizeof(UBO);
    descriptor.usage = BufferUsageFlagBits::kUniform;

    m_offscreen.uniformBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_offscreen.uniformBuffer->map();
    // memcpy(pointer, &m_ubo, descriptor.size);
    // m_offscreen.uniformBuffer->unmap();
}

void JuneVulkanService3::createOffscreenBindGroupLayout()
{
    BufferBindingLayout bufferLayout{};
    bufferLayout.index = 0;
    bufferLayout.stages = BindingStageFlagBits::kVertexStage;
    bufferLayout.type = BufferBindingType::kUniform;

    BindGroupLayoutDescriptor descriptor{};
    descriptor.buffers = { bufferLayout };

    m_offscreen.bindGroupLayout = m_device->createBindGroupLayout(descriptor);
}

void JuneVulkanService3::createOffscreenBindGroup()
{
    BufferBinding bufferBinding{
        .index = 0,
        .offset = 0,
        .size = m_offscreen.uniformBuffer->getSize(),
        .buffer = m_offscreen.uniformBuffer.get(),
    };

    BindGroupDescriptor descriptor{
        .layout = m_offscreen.bindGroupLayout.get(),
        .buffers = { bufferBinding }
    };

    m_offscreen.bindGroup = m_device->createBindGroup(descriptor);
}

void JuneVulkanService3::createOffscreenRenderPipeline()
{
    // render pipeline layout
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_offscreen.bindGroupLayout.get() };

        m_offscreen.renderPipelineLayout = m_device->createPipelineLayout(descriptor);
    }

    // input assembly stage
    InputAssemblyStage inputAssemblyStage{};
    {
        inputAssemblyStage.topology = PrimitiveTopology::kTriangleList;
    }

    // vertex shader module
    std::unique_ptr<ShaderModule> vertexShaderModule = nullptr;
    {
        std::vector<char> vertexShaderSource = utils::readFile(m_descriptor.appDir / "offscreen.vert.spv", m_descriptor.appHandle);
        ShaderModuleDescriptor descriptor{};
        descriptor.type = ShaderModuleType::kSPIRV;
        descriptor.code = std::string_view(vertexShaderSource.data(), vertexShaderSource.size());

        vertexShaderModule = m_device->createShaderModule(descriptor);
    }

    // vertex stage

    VertexAttribute positionAttribute{};
    positionAttribute.format = VertexFormat::kFloat32x3;
    positionAttribute.offset = offsetof(OffscreenVertex, pos);
    positionAttribute.location = 0;

    VertexAttribute colorAttribute{};
    colorAttribute.format = VertexFormat::kFloat32x3;
    colorAttribute.offset = offsetof(OffscreenVertex, color);
    colorAttribute.location = 1;

    VertexInputLayout vertexInputLayout{};
    vertexInputLayout.mode = VertexMode::kVertex;
    vertexInputLayout.stride = sizeof(OffscreenVertex);
    vertexInputLayout.attributes = { positionAttribute, colorAttribute };

    VertexStage vertexStage{
        { vertexShaderModule.get(), "main" },
        { vertexInputLayout }
    };

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
        std::vector<char> fragmentShaderSource = utils::readFile(m_descriptor.appDir / "offscreen.frag.spv", m_descriptor.appHandle);
        ShaderModuleDescriptor descriptor{};
        descriptor.type = ShaderModuleType::kSPIRV;
        descriptor.code = std::string_view(fragmentShaderSource.data(), fragmentShaderSource.size());

        fragmentShaderModule = m_device->createShaderModule(descriptor);
    }

    // fragment

    FragmentStage::Target target{};
    target.format = m_offscreen.renderTexture->getFormat();

    FragmentStage fragmentStage{
        { fragmentShaderModule.get(), "main" }, { target }
    };

    // depth/stencil

    // render pipeline
    RenderPipelineDescriptor descriptor{
        m_offscreen.renderPipelineLayout.get(),
        inputAssemblyStage,
        vertexStage,
        rasterizationStage,
        fragmentStage
    };

    m_offscreen.renderPipeline = m_device->createRenderPipeline(descriptor);
}

void JuneVulkanService3::createOnscreenImage()
{
    JuneGetResourceDescriptor juneGetResourceDescriptor{};
    m_onscreen.image = reinterpret_cast<VkImage>(m_juneAPI.ResourceGetResource(m_onscreen.resource,
                                                                               &juneGetResourceDescriptor));
    assert(m_onscreen.image);
}

void JuneVulkanService3::createOnscreenTexture()
{
    VulkanTextureDescriptor vulkanTextureDescriptor{};
    vulkanTextureDescriptor.imageType = VK_IMAGE_TYPE_2D;
#if defined(__ANDROID__) || defined(ANDROID)
    vulkanTextureDescriptor.format = VK_FORMAT_R8G8B8A8_UNORM;
#else
    vulkanTextureDescriptor.format = VK_FORMAT_B8G8R8A8_UNORM;
#endif
    vulkanTextureDescriptor.extent.width = m_descriptor.width;
    vulkanTextureDescriptor.extent.height = m_descriptor.height;
    vulkanTextureDescriptor.extent.depth = 1;
    vulkanTextureDescriptor.mipLevels = 1;
    vulkanTextureDescriptor.arrayLayers = 1;
    vulkanTextureDescriptor.samples = VK_SAMPLE_COUNT_1_BIT;
    vulkanTextureDescriptor.tiling = VK_IMAGE_TILING_LINEAR; // VK_IMAGE_TILING_OPTIMAL is better for performance. but size is larger.
    vulkanTextureDescriptor.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    vulkanTextureDescriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vulkanTextureDescriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vulkanTextureDescriptor.owner = VulkanTextureOwner::kExternal;
    vulkanTextureDescriptor.image = m_onscreen.image;
    // vulkanTextureDescriptor.flags = VK_IMAGE_CREATE_ALIAS_BIT;

    auto vulkanDevice = static_cast<VulkanDevice*>(m_device.get());
    m_onscreen.renderTexture = vulkanDevice->createTexture(vulkanTextureDescriptor);

    // image layout transition
    {
        auto vulkanRenderTexture = static_cast<VulkanTexture*>(m_onscreen.renderTexture.get());

        CommandEncoderDescriptor commandDescriptor{};
        auto commandEncoder = m_device->createCommandEncoder(commandDescriptor);
        auto vulkanCommandEncoder = static_cast<VulkanCommandEncoder*>(commandEncoder.get());

        {
            VkImageSubresourceRange range;
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.pNext = VK_NULL_HANDLE;
            barrier.srcAccessMask = VK_ACCESS_NONE;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; // or vulkanRenderTexture->getCurrentLayout(0);
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = vulkanRenderTexture->getVkImage();
            barrier.subresourceRange = range;

            VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

            vulkanCommandEncoder->imageTransition(vulkanRenderTexture, barrier, srcStage, dstStage);
        }

        auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
        m_queue->submit({ commandBuffer.get() });
    }
}

void JuneVulkanService3::createOnscreenTextureView()
{
    TextureViewDescriptor textureViewDescriptor;
    textureViewDescriptor.aspect = TextureAspectFlagBits::kColor;
    textureViewDescriptor.dimension = TextureViewDimension::k2D;

    m_onscreen.renderTextureView = m_onscreen.renderTexture->createTextureView(textureViewDescriptor);
}

void JuneVulkanService3::createOnscreenVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_onscreenVertices.size() * sizeof(OnscreenVertex);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_onscreen.vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_onscreen.vertexBuffer->map();
    memcpy(pointer, m_onscreenVertices.data(), descriptor.size);
    m_onscreen.vertexBuffer->unmap();
}

void JuneVulkanService3::createOnscreenIndexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_onscreenIndices.size() * sizeof(uint16_t);
    descriptor.usage = BufferUsageFlagBits::kIndex;

    m_onscreen.indexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_onscreen.indexBuffer->map();
    memcpy(pointer, m_onscreenIndices.data(), descriptor.size);
    m_onscreen.indexBuffer->unmap();
}

void JuneVulkanService3::createOnscreenSampler()
{
    SamplerDescriptor samplerDescriptor{};
    samplerDescriptor.addressModeU = AddressMode::kClampToEdge;
    samplerDescriptor.addressModeV = AddressMode::kClampToEdge;
    samplerDescriptor.addressModeW = AddressMode::kClampToEdge;
    samplerDescriptor.lodMin = 0.0f;
    samplerDescriptor.lodMax = static_cast<float>(m_onscreen.renderTexture->getMipLevels());
    samplerDescriptor.minFilter = FilterMode::kLinear;
    samplerDescriptor.magFilter = FilterMode::kLinear;
    samplerDescriptor.mipmapFilter = MipmapFilterMode::kLinear;

    m_onscreen.sampler = m_device->createSampler(samplerDescriptor);
}

void JuneVulkanService3::createOnscreenBindGroupLayout()
{
    SamplerBindingLayout samplerLayout{};
    samplerLayout.index = 0;
    samplerLayout.stages = BindingStageFlagBits::kFragmentStage;

    TextureBindingLayout textureLayout{};
    textureLayout.index = 1;
    textureLayout.stages = BindingStageFlagBits::kFragmentStage;

    BindGroupLayoutDescriptor descriptor{};
    descriptor.samplers = { samplerLayout };
    descriptor.textures = { textureLayout };

    m_onscreen.bindGroupLayout = m_device->createBindGroupLayout(descriptor);
}

void JuneVulkanService3::createOnscreenBindGroup()
{
    SamplerBinding samplerBinding{
        .index = 0,
        .sampler = m_onscreen.sampler.get()
    };

    TextureBinding textureBinding{
        .index = 1,
        .textureView = m_onscreen.renderTextureView.get()
    };

    BindGroupDescriptor descriptor{
        .layout = m_onscreen.bindGroupLayout.get(),
        .samplers = { samplerBinding },
        .textures = { textureBinding }
    };

    m_onscreen.bindGroup = m_device->createBindGroup(descriptor);
}

void JuneVulkanService3::createOnscreenRenderPipeline()
{
    // render pipeline layout
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_onscreen.bindGroupLayout.get() };

        m_onscreen.renderPipelineLayout = m_device->createPipelineLayout(descriptor);
    }

    // input assembly stage
    InputAssemblyStage inputAssemblyStage{};
    {
        inputAssemblyStage.topology = PrimitiveTopology::kTriangleList;
    }

    // vertex shader module
    std::unique_ptr<ShaderModule> vertexShaderModule = nullptr;
    {
        std::vector<char> vertexShaderSource = utils::readFile(m_descriptor.appDir / "onscreen.vert.spv", m_descriptor.appHandle);
        ShaderModuleDescriptor descriptor{};
        descriptor.type = ShaderModuleType::kSPIRV;
        descriptor.code = std::string_view(vertexShaderSource.data(), vertexShaderSource.size());

        vertexShaderModule = m_device->createShaderModule(descriptor);
    }

    // vertex stage

    VertexAttribute positionAttribute{};
    positionAttribute.format = VertexFormat::kFloat32x3;
    positionAttribute.offset = offsetof(OnscreenVertex, pos);
    positionAttribute.location = 0;

    VertexAttribute texCoordAttribute{};
    texCoordAttribute.format = VertexFormat::kFloat32x2;
    texCoordAttribute.offset = offsetof(OnscreenVertex, texCoord);
    texCoordAttribute.location = 1;

    VertexInputLayout vertexInputLayout{};
    vertexInputLayout.mode = VertexMode::kVertex;
    vertexInputLayout.stride = sizeof(OnscreenVertex);
    vertexInputLayout.attributes = { positionAttribute, texCoordAttribute };

    VertexStage vertexStage{
        { vertexShaderModule.get(), "main" },
        { vertexInputLayout }
    };

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
        std::vector<char> fragmentShaderSource = utils::readFile(m_descriptor.appDir / "onscreen.frag.spv", m_descriptor.appHandle);
        ShaderModuleDescriptor descriptor{};
        descriptor.type = ShaderModuleType::kSPIRV;
        descriptor.code = std::string_view(fragmentShaderSource.data(), fragmentShaderSource.size());

        fragmentShaderModule = m_device->createShaderModule(descriptor);
    }

    // fragment

    FragmentStage::Target target{};
    target.format = m_swapchain->getTextureFormat();

    FragmentStage fragmentStage{
        { fragmentShaderModule.get(), "main" },
        { target }
    };

    // depth/stencil

    // render pipeline
    RenderPipelineDescriptor descriptor{
        m_onscreen.renderPipelineLayout.get(),
        inputAssemblyStage,
        vertexStage,
        rasterizationStage,
        fragmentStage
    };

    m_onscreen.renderPipeline = m_device->createRenderPipeline(descriptor);
}

void JuneVulkanService3::createCamera()
{
    m_camera = std::make_unique<PerspectiveCamera>(45.0f,
                                                   m_descriptor.width / static_cast<float>(m_descriptor.height),
                                                   0.1f,
                                                   1000.0f);

    // auto halfWidth = m_descriptor.width / 2.0f;
    // auto halfHeight = m_descriptor.height / 2.0f;
    // m_camera = std::make_unique<OrthographicCamera>(-halfWidth, halfWidth,
    //                                                 -halfHeight, halfHeight,
    //                                                 -1000, 1000);

    m_camera->lookAt(glm::vec3(0.0f, 0.0f, 1000.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0));
}

void JuneVulkanService3::updateOffscreenUniformBuffer()
{
    m_ubo.mvp.model = glm::mat4(1.0f);
    m_ubo.mvp.view = m_camera->getViewMat();
    m_ubo.mvp.proj = m_camera->getProjectionMat();

    void* pointer = m_offscreen.uniformBuffer->map(); // do not unmap.
    memcpy(pointer, &m_ubo, m_offscreen.uniformBuffer->getSize());
}

} // namespace jipu
