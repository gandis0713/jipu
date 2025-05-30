#include "june_vulkan_service2.h"

#include <spdlog/spdlog.h>

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <android/sync.h>
#endif

namespace jipu
{

JuneVulkanService2::JuneVulkanService2(const JuneServiceDescriptor& descriptor)
    : JuneVulkanService(descriptor)
{
}

JuneVulkanService2::~JuneVulkanService2()
{
}

void JuneVulkanService2::begin()
{
    JuneVulkanService::begin();

    // Create Fence
    {
        JuneFenceCreateDescriptor fenceDescriptor{};
        fenceDescriptor.type = JuneFenceType_SyncFD;
        m_signalFence = m_juneAPI.InstanceCreateFence(m_juneInstance, &fenceDescriptor);
    }
}

void JuneVulkanService2::work()
{
    auto sharedMemories = getSharedMemories();
    if (sharedMemories.empty())
        return;

    CommandEncoderDescriptor commandDescriptor{};
    auto commandEncoder = m_device->createCommandEncoder(commandDescriptor);
    auto vulkanCommandEncoder = static_cast<VulkanCommandEncoder*>(commandEncoder.get());

    {
        auto onscreenReadTextureView = m_onscreen.renderTextureView.get();
        auto onscreenReadVulkanRenderTexture = static_cast<VulkanTexture*>(onscreenReadTextureView->getTexture());

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
    }

    auto renderView = m_swapchain->acquireNextTextureView();

    ColorAttachment attachment{
        .renderView = renderView
    };
    attachment.clearValue = { 0.0, 0.0, 0.0, 0.0 };
    attachment.loadOp = LoadOp::kClear;
    attachment.storeOp = StoreOp::kStore;

    RenderPassEncoderDescriptor renderPassDescriptor{
        .colorAttachments = { attachment }
    };

    auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassDescriptor);
    renderPassEncoder->setPipeline(m_onscreen.renderPipeline.get());
    renderPassEncoder->setBindGroup(0, m_onscreen.bindGroup.get());
    renderPassEncoder->setVertexBuffer(0, m_onscreen.vertexBuffer.get());
    renderPassEncoder->setIndexBuffer(m_onscreen.indexBuffer.get(), IndexFormat::kUint16);
    renderPassEncoder->setScissor(0, 0, m_descriptor.width, m_descriptor.height);
    renderPassEncoder->setViewport(0, 0, m_descriptor.width, m_descriptor.height, 0, 1);
    renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_onscreenIndices.size()), 1, 0, 0, 0);
    renderPassEncoder->end();

    {
        auto onscreenReadTextureView = m_onscreen.renderTextureView.get();
        auto onscreenReadVulkanRenderTexture = static_cast<VulkanTexture*>(onscreenReadTextureView->getTexture());

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
        barrier.dstAccessMask = VK_ACCESS_NONE;
        barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // or onscreenReadVulkanRenderTexture->getCurrentLayout(0);
        barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = onscreenReadVulkanRenderTexture->getVkImage();
        barrier.subresourceRange = range;

        VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        vulkanCommandEncoder->imageTransition(onscreenReadVulkanRenderTexture, barrier, srcStage, dstStage);
    }

    auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
    auto vulkanQueue = static_cast<VulkanQueue*>(m_queue.get());

    std::vector<VkSemaphore> waitSemaphore{};
    {
        std::vector<JuneFence> waitFences = getWaitFences();
        for (const auto& fence : waitFences)
        {
            JuneFenceVkSemaphoreExportDescriptor vkSemaphoreExportDescriptor{};
            vkSemaphoreExportDescriptor.chain.sType = JuneSType_FenceVkSemaphoreExportDescriptor;

            JuneFenceExportDescriptor descriptor{};
            descriptor.nextInChain = &vkSemaphoreExportDescriptor.chain;
            descriptor.fence = fence;

            m_juneAPI.ApiContextExportFence(m_juneApiContext, &descriptor);
            if (!vkSemaphoreExportDescriptor.vkSemaphore)
            {
                spdlog::trace("VkSemaphore null in vulkan service 2: {:p}", vkSemaphoreExportDescriptor.vkSemaphore);
                continue;
            }
            waitSemaphore.push_back(reinterpret_cast<VkSemaphore>(vkSemaphoreExportDescriptor.vkSemaphore));
        }
    }
    std::vector<VkPipelineStageFlags> waitStages(waitSemaphore.size(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    spdlog::debug("vulkan service2 begin access");

    VulkanSubmitContext submitContext = VulkanSubmitContext::create(static_cast<VulkanDevice*>(m_device.get()), { commandBuffer.get() });

    auto& submits = submitContext.getSubmitsRef();
    for (auto& submit : submits)
    {
        submit.addWaitSemaphore(waitSemaphore, waitStages);
    }
    vulkanQueue->submit(submitContext);
    m_swapchain->present();

    spdlog::debug("vulkan service2 end access");
}

void JuneVulkanService2::addSharedMemory(JuneSharedMemory sharedMemory)
{
    std::lock_guard<std::mutex> lock(m_sharedMemoryMutex);
    m_sharedMemories.push_back(sharedMemory);

    // Create Resource
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

        JuneResourceVkImageCreateInfo vkImageCreateInfo;
        vkImageCreateInfo.vkImageCreateInfo = &imageInfo;

        JuneResourceVkImageResultInfo vkImageResultInfo;

        JuneResourceVkImageCreateDescriptor juneResourceVkImageDescriptor{};
        juneResourceVkImageDescriptor.chain.sType = JuneSType_ResourceVkImageCreateDescriptor;
        juneResourceVkImageDescriptor.createInfo = &vkImageCreateInfo;
        juneResourceVkImageDescriptor.resultInfo = &vkImageResultInfo;

        JuneResourceCreateDescriptor juneResourceDescriptor{};
        juneResourceDescriptor.nextInChain = &juneResourceVkImageDescriptor.chain;
        juneResourceDescriptor.sharedMemory = sharedMemory;

        m_juneAPI.ApiContextCreateResource(m_juneApiContext, &juneResourceDescriptor);

        m_onscreen.image = reinterpret_cast<VkImage>(vkImageResultInfo.vkImage);
        m_onscreen.deviceMemory = reinterpret_cast<VkDeviceMemory>(vkImageResultInfo.vkDeviceMemory);
    }

    createOnscreenTexture();
    createOnscreenTextureView();

    createOnscreenVertexBuffer();
    createOnscreenIndexBuffer();
    createOnscreenSampler();
    createOnscreenBindGroupLayout();
    createOnscreenBindGroup();
    createOnscreenRenderPipeline();
}

void JuneVulkanService2::createOnscreenTexture()
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
    vulkanTextureDescriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
    vulkanTextureDescriptor.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    vulkanTextureDescriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vulkanTextureDescriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vulkanTextureDescriptor.owner = VulkanTextureOwner::kExternal;
    vulkanTextureDescriptor.image = m_onscreen.image;

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
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; // or vulkanRenderTexture->getCurrentLayout(0);
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = vulkanRenderTexture->getVkImage();
            barrier.subresourceRange = range;

            VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

            vulkanCommandEncoder->imageTransition(vulkanRenderTexture, barrier, srcStage, dstStage);
        }

        auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
        m_queue->submit({ commandBuffer.get() });
    }
}

void JuneVulkanService2::createOnscreenTextureView()
{
    TextureViewDescriptor textureViewDescriptor;
    textureViewDescriptor.aspect = TextureAspectFlagBits::kColor;
    textureViewDescriptor.dimension = TextureViewDimension::k2D;

    m_onscreen.renderTextureView = m_onscreen.renderTexture->createTextureView(textureViewDescriptor);
}

void JuneVulkanService2::createOnscreenVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_onscreenVertices.size() * sizeof(OnscreenVertex);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_onscreen.vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_onscreen.vertexBuffer->map();
    memcpy(pointer, m_onscreenVertices.data(), descriptor.size);
    m_onscreen.vertexBuffer->unmap();
}

void JuneVulkanService2::createOnscreenIndexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_onscreenIndices.size() * sizeof(uint16_t);
    descriptor.usage = BufferUsageFlagBits::kIndex;

    m_onscreen.indexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_onscreen.indexBuffer->map();
    memcpy(pointer, m_onscreenIndices.data(), descriptor.size);
    m_onscreen.indexBuffer->unmap();
}

void JuneVulkanService2::createOnscreenSampler()
{
    SamplerDescriptor samplerDescriptor{};
    samplerDescriptor.addressModeU = AddressMode::kClampToEdge;
    samplerDescriptor.addressModeV = AddressMode::kClampToEdge;
    samplerDescriptor.addressModeW = AddressMode::kClampToEdge;
    samplerDescriptor.lodMin = 0.0f;
    samplerDescriptor.lodMax = 1.0f;
    samplerDescriptor.minFilter = FilterMode::kLinear;
    samplerDescriptor.magFilter = FilterMode::kLinear;
    samplerDescriptor.mipmapFilter = MipmapFilterMode::kLinear;

    m_onscreen.sampler = m_device->createSampler(samplerDescriptor);
}

void JuneVulkanService2::createOnscreenBindGroupLayout()
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

void JuneVulkanService2::createOnscreenBindGroup()
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

void JuneVulkanService2::createOnscreenRenderPipeline()
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

} // namespace jipu
