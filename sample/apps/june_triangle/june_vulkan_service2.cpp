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
}

void JuneVulkanService2::work()
{
    if (!m_isShared)
        return;
}

JuneServiceShareObjects JuneVulkanService2::getSharingObject() const
{
    return m_sharingObjects;
}

void JuneVulkanService2::setSharedObjects(const JuneServiceShareObjects& sharedObjects)
{
    m_sharedObjects = sharedObjects;
    m_sharingObjects.sharedMemory = sharedObjects.sharedMemory;

    // Create ApiMemory and connect
    JuneApiMemory apiMemory{};
    {
        JuneApiMemoryDescriptor juneApiMemoryDescriptor{};
        juneApiMemoryDescriptor.nextInChain = nullptr;
        juneApiMemoryDescriptor.sharedMemory = m_sharedObjects.sharedMemory;

        apiMemory = m_juneAPI.ApiContextCreateApiMemory(m_juneApiContext, &juneApiMemoryDescriptor);
        m_sharingObjects.apiMemories.push_back(apiMemory);

        // for (const auto& sharedApiMemory : m_sharedObjects.apiMemories)
        // {
        //     m_juneAPI.ApiMemoryConnect(sharedApiMemory, m_juneApiMemory);
        //     m_juneAPI.ApiMemoryConnect(m_juneApiMemory, sharedApiMemory);
        // }
    }

    // Create Resource
    {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.extent.width = m_descriptor.width;
        imageInfo.extent.height = m_descriptor.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        JuneResourceVkImageDescriptor juneResourceVkImageDescriptor{};
        juneResourceVkImageDescriptor.chain.sType = JuneSType_VkImageResourceDescriptor;
        juneResourceVkImageDescriptor.vkImageCreateInfo = &imageInfo;

        JuneResourceDescriptor juneResourceDescriptor{};
        juneResourceDescriptor.nextInChain = &juneResourceVkImageDescriptor.chain;

        m_image = reinterpret_cast<VkImage>(m_juneAPI.ApiMemoryCreateResource(apiMemory, &juneResourceDescriptor));
        assert(m_image);
    }

    createOffscreenTexture();
    createOffscreenTextureView();

    createOnscreenVertexBuffer();
    createOnscreenIndexBuffer();
    createOnscreenSampler();
    createOnscreenBindGroupLayout();
    createOnscreenBindGroup();
    createOnscreenRenderPipeline();

    m_isShared = true;
}

void JuneVulkanService2::createOffscreenTexture()
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
    vulkanTextureDescriptor.image = m_image;

    auto vulkanDevice = static_cast<VulkanDevice*>(m_device.get());
    m_offscreen.renderTexture = vulkanDevice->createTexture(vulkanTextureDescriptor);
}

void JuneVulkanService2::createOffscreenTextureView()
{
    TextureViewDescriptor textureViewDescriptor;
    textureViewDescriptor.aspect = TextureAspectFlagBits::kColor;
    textureViewDescriptor.dimension = TextureViewDimension::k2D;

    m_offscreen.renderTextureView = m_offscreen.renderTexture->createTextureView(textureViewDescriptor);
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
        .textureView = m_offscreen.renderTextureView.get()
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
