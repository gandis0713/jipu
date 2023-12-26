

#include "file.h"
#include "im_gui.h"
#include "image.h"
#include "model.h"
#include "sample.h"

#include "jipu/binding_group.h"
#include "jipu/binding_group_layout.h"
#include "jipu/buffer.h"
#include "jipu/command_buffer.h"
#include "jipu/device.h"
#include "jipu/driver.h"
#include "jipu/physical_device.h"
#include "jipu/pipeline.h"
#include "jipu/pipeline_layout.h"
#include "jipu/queue.h"
#include "jipu/sampler.h"
#include "jipu/shader_module.h"
#include "jipu/surface.h"
#include "jipu/swapchain.h"
#include "jipu/texture_view.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <spdlog/spdlog.h>
#include <stddef.h>

namespace jipu
{

class OBJModelSample : public Sample, public Im_Gui
{
public:
    OBJModelSample() = delete;
    OBJModelSample(const SampleDescriptor& descriptor);
    ~OBJModelSample() override;

    void init() override;
    void update() override;
    void draw() override;

private:
    void updateImGui() override;

private:
    void createSwapchain();

    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffer();

    void createImageTexture();
    void createImageTextureView();
    void createImageSampler();

    void createColorAttachmentTexture();
    void createColorAttachmentTextureView();
    void createDepthStencilTexture();
    void createDepthStencilTextureView();

    void createBindingGroupLayout();
    void createBindingGroup();

    void createPipelineLayout();
    void createRenderPipeline();
    void createCommandBuffers();

    void copyBufferToBuffer(Buffer* src, Buffer* dst);
    void copyBufferToTexture(Buffer* imageTextureBuffer, Texture* imageTexture);

    void updateUniformBuffer();

private:
    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    // data
    Polygon m_polygon{};
    std::unique_ptr<Image> m_image = nullptr;

    // wrapper
    std::unique_ptr<Driver> m_driver = nullptr;
    std::vector<std::unique_ptr<PhysicalDevice>> m_physicalDevices{};

    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;

    std::unique_ptr<Queue> m_queue = nullptr;

    std::unique_ptr<Swapchain> m_swapchain = nullptr;

    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<Buffer> m_indexBuffer = nullptr;

    std::unique_ptr<Texture> m_imageTexture = nullptr;
    std::unique_ptr<TextureView> m_imageTextureView = nullptr;
    std::unique_ptr<Sampler> m_imageSampler = nullptr;

    std::unique_ptr<Buffer> m_uniformBuffer = nullptr;
    void* m_uniformBufferMappedPointer = nullptr;

    std::unique_ptr<Texture> m_colorAttachmentTexture = nullptr;
    std::unique_ptr<TextureView> m_colorAttachmentTextureView = nullptr;
    std::unique_ptr<Texture> m_depthStencilTexture = nullptr;
    std::unique_ptr<TextureView> m_depthStencilTextureView = nullptr;

    std::unique_ptr<BindingGroupLayout> m_bindingGroupLayout = nullptr;
    std::unique_ptr<BindingGroup> m_bindingGroup = nullptr;

    std::unique_ptr<PipelineLayout> m_pipelineLayout = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    std::unique_ptr<ShaderModule> m_vertexShaderModule = nullptr;
    std::unique_ptr<ShaderModule> m_fragmentShaderModule = nullptr;

    std::unique_ptr<CommandBuffer> m_renderCommandBuffer = nullptr;

    uint32_t m_sampleCount = 4;
};

OBJModelSample::OBJModelSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

OBJModelSample::~OBJModelSample()
{
    clearImGui();

    m_vertexShaderModule.reset();
    m_fragmentShaderModule.reset();

    m_renderPipeline.reset();
    m_pipelineLayout.reset();

    m_bindingGroupLayout.reset();
    m_bindingGroup.reset();

    m_depthStencilTextureView.reset();
    m_depthStencilTexture.reset();
    m_colorAttachmentTextureView.reset();
    m_colorAttachmentTexture.reset();

    // unmap m_uniformBufferMappedPointer;
    m_uniformBuffer.reset();

    m_imageSampler.reset();
    m_imageTextureView.reset();
    m_imageTexture.reset();

    m_indexBuffer.reset();
    m_vertexBuffer.reset();

    m_queue.reset();

    // release command buffer after finising queue.
    m_renderCommandBuffer.reset();
    m_swapchain.reset();

    m_physicalDevices.clear();
    m_device.reset();

    m_surface.reset();
    m_driver.reset();
}

void OBJModelSample::init()
{
    // create Driver.
    {
        DriverDescriptor descriptor{ .type = DriverType::kVulkan };
        m_driver = Driver::create(descriptor);
    }

    // create surface
    {
        SurfaceDescriptor descriptor{ .windowHandle = getWindowHandle() };
        m_surface = m_driver->createSurface(descriptor);
    }

    // create PhysicalDevice.
    {
        m_physicalDevices = m_driver->getPhysicalDevices();
    }

    // create Device.
    {
        // TODO: select suit device.
        PhysicalDevice* physicalDevice = m_physicalDevices[0].get();

        DeviceDescriptor descriptor;
        m_device = physicalDevice->createDevice(descriptor);
    }

    // create queue
    {
        QueueDescriptor rednerQueueDescriptor{ .flags = QueueFlagBits::kGraphics | QueueFlagBits::kTransfer };
        m_queue = m_device->createQueue(rednerQueueDescriptor);
    }

    // create swapchain
    createSwapchain();

    // create buffer
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffer();

    createImageTexture();
    createImageTextureView();
    createImageSampler();

    createColorAttachmentTexture();
    createColorAttachmentTextureView();
    createDepthStencilTexture();
    createDepthStencilTextureView();

    createBindingGroupLayout();
    createBindingGroup();

    createPipelineLayout();
    createRenderPipeline();
    createCommandBuffers();

    initImGui(m_device.get(), m_queue.get(), m_swapchain.get());

    m_initialized = true;
}

void OBJModelSample::update()
{
    updateUniformBuffer();

    updateImGui();
    buildImGui();
}

void OBJModelSample::updateImGui()
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
        ImGui::End();
    }

    debugWindow();
    ImGui::Render();
}

void OBJModelSample::draw()
{
    std::vector<TextureView*> swapchainTextureViews = m_swapchain->getTextureViews();
    int nextImageIndex = m_swapchain->acquireNextTexture();

    CommandEncoderDescriptor commandEncoderDescriptor{};
    std::unique_ptr<CommandEncoder> commandEncoder = m_renderCommandBuffer->createCommandEncoder(commandEncoderDescriptor);

    std::vector<ColorAttachment> colorAttachments(1); // in currently. use only one.
    colorAttachments[0] = { .renderView = m_sampleCount > 1 ? m_colorAttachmentTextureView.get() : swapchainTextureViews[nextImageIndex],
                            .resolveView = m_sampleCount > 1 ? swapchainTextureViews[nextImageIndex] : nullptr,
                            .loadOp = LoadOp::kClear,
                            .storeOp = StoreOp::kStore,
                            .clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } } };
    DepthStencilAttachment depthStencilAttachment{ .textureView = m_depthStencilTextureView.get(),
                                                   .depthLoadOp = LoadOp::kClear,
                                                   .depthStoreOp = StoreOp::kStore,
                                                   .stencilLoadOp = LoadOp::kDontCare,
                                                   .stencilStoreOp = StoreOp::kDontCare,
                                                   .clearValue = { .depth = 1.0f, .stencil = 0 } };

    RenderPassEncoderDescriptor renderPassEncoderDescriptor{ .colorAttachments = colorAttachments,
                                                             .depthStencilAttachment = depthStencilAttachment,
                                                             .sampleCount = m_sampleCount };

    std::unique_ptr<RenderPassEncoder> renderPassEncoder = commandEncoder->beginRenderPass(renderPassEncoderDescriptor);
    renderPassEncoder->setPipeline(m_renderPipeline.get());
    renderPassEncoder->setBindingGroup(0, m_bindingGroup.get());
    renderPassEncoder->setVertexBuffer(0, m_vertexBuffer.get());
    renderPassEncoder->setIndexBuffer(m_indexBuffer.get(), IndexFormat::kUint16);
    renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1); // set viewport state.
    renderPassEncoder->setScissor(0, 0, m_width, m_height);        // set scissor state.
    renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_polygon.indices.size()), 1, 0, 0, 0);
    renderPassEncoder->end();

    drawImGui(commandEncoder.get(), swapchainTextureViews[nextImageIndex]);

    m_queue->submit({ commandEncoder->finish() }, m_swapchain.get());
}

void OBJModelSample::createSwapchain()
{
#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat textureFormat = TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
#else
    TextureFormat textureFormat = TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
#endif
    SwapchainDescriptor descriptor{ .textureFormat = textureFormat,
                                    .presentMode = PresentMode::kFifo,
                                    .colorSpace = ColorSpace::kSRGBNonLinear,
                                    .width = m_width,
                                    .height = m_height,
                                    .surface = m_surface.get() };
    m_swapchain = m_device->createSwapchain(descriptor);
}

void OBJModelSample::createVertexBuffer()
{
    // load obj as buffer for android.
    std::vector<char> buffer = utils::readFile(m_appDir / "viking_room.obj", m_handle);
    m_polygon = loadOBJ(buffer.data(), buffer.size());

    uint64_t vertexSize = static_cast<uint64_t>(sizeof(Vertex) * m_polygon.vertices.size());
    BufferDescriptor vertexStagingBufferDescriptor{ .size = vertexSize,
                                                    .usage = BufferUsageFlagBits::kCopySrc };
    std::unique_ptr<Buffer> vertexStagingBuffer = m_device->createBuffer(vertexStagingBufferDescriptor);

    void* mappedPointer = vertexStagingBuffer->map();
    memcpy(mappedPointer, m_polygon.vertices.data(), vertexSize);

    BufferDescriptor vertexBufferDescriptor{ .size = vertexSize,
                                             .usage = BufferUsageFlagBits::kVertex | BufferUsageFlagBits::kCopyDst };
    m_vertexBuffer = m_device->createBuffer(vertexBufferDescriptor);

    // copy staging buffer to target buffer
    copyBufferToBuffer(vertexStagingBuffer.get(), m_vertexBuffer.get());

    // unmap staging buffer.
    // vertexStagingBuffer->unmap(); // TODO: need unmap before destroy it?
}

void OBJModelSample::createIndexBuffer()
{
    uint64_t indexSize = static_cast<uint64_t>(sizeof(uint16_t) * m_polygon.indices.size());
    BufferDescriptor indexBufferDescriptor{ .size = indexSize,
                                            .usage = BufferUsageFlagBits::kIndex };

    m_indexBuffer = m_device->createBuffer(indexBufferDescriptor);

    void* mappedPointer = m_indexBuffer->map();
    memcpy(mappedPointer, m_polygon.indices.data(), indexSize);
    m_indexBuffer->unmap();
}

void OBJModelSample::createUniformBuffer()
{
    BufferDescriptor descriptor{ .size = sizeof(UniformBufferObject),
                                 .usage = BufferUsageFlagBits::kUniform };
    m_uniformBuffer = m_device->createBuffer(descriptor);
    m_uniformBufferMappedPointer = m_uniformBuffer->map();
}

void OBJModelSample::createImageTexture()
{
    // load as buffer for android.
    std::vector<char> buffer = utils::readFile(m_appDir / "viking_room.png", m_handle);
    m_image = std::make_unique<Image>(buffer.data(), buffer.size());

    unsigned char* pixels = static_cast<unsigned char*>(m_image->getPixels());
    uint32_t width = m_image->getWidth();
    uint32_t height = m_image->getHeight();
    uint32_t channel = m_image->getChannel();
    uint64_t imageSize = sizeof(unsigned char) * width * height * channel;

    uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

    // create image staging buffer.
    BufferDescriptor descriptor{ .size = imageSize, .usage = BufferUsageFlagBits::kCopySrc };
    std::unique_ptr<Buffer> imageTextureStagingBuffer = m_device->createBuffer(descriptor);

    void* mappedPointer = imageTextureStagingBuffer->map();
    memcpy(mappedPointer, pixels, imageSize);

    // create texture.
    TextureDescriptor textureDescriptor{ .type = TextureType::k2D,
                                         .format = TextureFormat::kRGBA_8888_UInt_Norm_SRGB,
                                         .usage = TextureUsageFlagBits::kCopySrc |
                                                  TextureUsageFlagBits::kCopyDst |
                                                  TextureUsageFlagBits::kTextureBinding,
                                         .width = width,
                                         .height = height,
                                         .depth = 1,
                                         .mipLevels = mipLevels,
                                         .sampleCount = 1 };
    m_imageTexture = m_device->createTexture(textureDescriptor);

    // copy image staging buffer to texture
    copyBufferToTexture(imageTextureStagingBuffer.get(), m_imageTexture.get());

    // unmap staging buffer
    // imageTextureStagingBuffer->unmap(); // TODO: need unmap before destroy it?
}

void OBJModelSample::createImageTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_imageTextureView = m_imageTexture->createTextureView(descriptor);
}

void OBJModelSample::createColorAttachmentTexture()
{
    // create color texture.
    TextureDescriptor textureDescriptor{ .type = TextureType::k2D,
                                         .format = m_swapchain->getTextureFormat(),
                                         .usage = TextureUsageFlagBits::kColorAttachment,
                                         .width = m_swapchain->getWidth(),
                                         .height = m_swapchain->getHeight(),
                                         .depth = 1,
                                         .mipLevels = 1,
                                         .sampleCount = m_sampleCount };
    m_colorAttachmentTexture = m_device->createTexture(textureDescriptor);
}

void OBJModelSample::createColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_colorAttachmentTextureView = m_colorAttachmentTexture->createTextureView(descriptor);
}

void OBJModelSample::createDepthStencilTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kD_32_SFloat;
    descriptor.usage = TextureUsageFlagBits::kDepthStencil;
    descriptor.mipLevels = 1;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.depth = 1;
    descriptor.sampleCount = m_sampleCount;

    m_depthStencilTexture = m_device->createTexture(descriptor);
}

void OBJModelSample::createDepthStencilTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kDepth;

    m_depthStencilTextureView = m_depthStencilTexture->createTextureView(descriptor);
}

void OBJModelSample::createImageSampler()
{
    SamplerDescriptor descriptor{};
    descriptor.magFilter = FilterMode::kLinear;
    descriptor.minFilter = FilterMode::kLinear;
    descriptor.mipmapFilter = MipmapFilterMode::kLinear;
    descriptor.addressModeU = AddressMode::kClampToEdge;
    descriptor.addressModeV = AddressMode::kClampToEdge;
    descriptor.addressModeW = AddressMode::kClampToEdge;
    descriptor.lodMin = 0.0f;
    // descriptor.lodMin = static_cast<float>(m_imageTexture->getMipLevels() / 2);
    descriptor.lodMax = static_cast<float>(m_imageTexture->getMipLevels());

    m_imageSampler = m_device->createSampler(descriptor);
}

void OBJModelSample::createBindingGroupLayout()
{
    // Uniform Buffer
    BufferBindingLayout bufferBindingLayout{};
    bufferBindingLayout.type = BufferBindingType::kUniform;
    bufferBindingLayout.index = 0;
    bufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;
    std::vector<BufferBindingLayout> bufferBindingLayouts{ bufferBindingLayout };

    // Sampler
    SamplerBindingLayout samplerBindingLayout{};
    samplerBindingLayout.index = 1;
    samplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
    samplerBindingLayout.withTexture = true;
    std::vector<SamplerBindingLayout> samplerBindingLayouts{ samplerBindingLayout };

    BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{ .buffers = bufferBindingLayouts,
                                                               .samplers = samplerBindingLayouts };

    m_bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
}

void OBJModelSample::createBindingGroup()
{
    const std::vector<BufferBindingLayout>& bufferBindingLayouts = m_bindingGroupLayout->getBufferBindingLayouts();
    std::vector<BufferBinding> bufferBindings{};
    bufferBindings.resize(bufferBindingLayouts.size());
    for (auto i = 0; i < bufferBindings.size(); ++i)
    {
        BufferBinding bufferBinding{};
        bufferBinding.index = bufferBindingLayouts[i].index;
        bufferBinding.buffer = m_uniformBuffer.get();
        bufferBinding.offset = 0;
        bufferBinding.size = sizeof(UniformBufferObject);

        bufferBindings[i] = bufferBinding;
    }

    const std::vector<SamplerBindingLayout>& samplerBindingLayouts = m_bindingGroupLayout->getSamplerBindingLayouts();
    std::vector<SamplerBinding> samplerBindings{};
    samplerBindings.resize(samplerBindingLayouts.size());
    for (auto i = 0; i < samplerBindings.size(); ++i)
    {
        SamplerBinding samplerBinding{};
        samplerBinding.index = samplerBindingLayouts[i].index;
        samplerBinding.sampler = m_imageSampler.get();
        samplerBinding.textureView = m_imageTextureView.get();

        samplerBindings[i] = samplerBinding;
    }

    std::vector<TextureBinding> textureBindings{};

    BindingGroupDescriptor descriptor{};
    descriptor.layout = m_bindingGroupLayout.get();
    descriptor.buffers = bufferBindings;
    descriptor.samplers = samplerBindings;
    descriptor.textures = textureBindings;

    m_bindingGroup = m_device->createBindingGroup(descriptor);
}

void OBJModelSample::createPipelineLayout()
{
    PipelineLayoutDescriptor pipelineLayoutDescriptor{ .layouts = { m_bindingGroupLayout.get() } };
    m_pipelineLayout = m_device->createPipelineLayout(pipelineLayoutDescriptor);
}

void OBJModelSample::createRenderPipeline()
{

    // Input Assembly
    InputAssemblyStage inputAssembly{};
    {
        inputAssembly.topology = PrimitiveTopology::kTriangleList;
    }

    // vertex stage
    VertexStage vertexStage{};
    {
        // create vertex shader
        const std::vector<char> vertShaderCode = utils::readFile(m_appDir / "obj_model.vert.spv", m_handle);
        ShaderModuleDescriptor vertexShaderModuleDescriptor{ .code = vertShaderCode.data(),
                                                             .codeSize = vertShaderCode.size() };
        m_vertexShaderModule = m_device->createShaderModule(vertexShaderModuleDescriptor);
        vertexStage.shaderModule = m_vertexShaderModule.get();

        // layouts
        std::vector<VertexInputLayout> layouts{};
        layouts.resize(1);
        {
            // attributes
            std::vector<VertexAttribute> vertexAttributes{};
            vertexAttributes.resize(2);
            {
                // position
                vertexAttributes[0] = { .format = VertexFormat::kSFLOATx3,
                                        .offset = offsetof(Vertex, pos),
                                        .location = 0 };

                // texture coodinate
                vertexAttributes[1] = { .format = VertexFormat::kSFLOATx2,
                                        .offset = offsetof(Vertex, texCoord),
                                        .location = 1 };
            }

            VertexInputLayout vertexLayout{ .mode = VertexMode::kVertex,
                                            .stride = sizeof(Vertex),
                                            .attributes = vertexAttributes };
            layouts[0] = vertexLayout;
        }

        vertexStage.layouts = layouts;
    }

    // Rasterization
    RasterizationStage rasterization{};
    {
        rasterization.sampleCount = m_sampleCount;
        rasterization.cullMode = CullMode::kNone;
        rasterization.frontFace = FrontFace::kCounterClockwise;
    }

    // fragment stage
    FragmentStage fragmentStage{};
    {
        // create fragment shader
        const std::vector<char> fragShaderCode = utils::readFile(m_appDir / "obj_model.frag.spv", m_handle);
        ShaderModuleDescriptor fragmentShaderModuleDescriptor{ .code = fragShaderCode.data(),
                                                               .codeSize = fragShaderCode.size() };
        m_fragmentShaderModule = m_device->createShaderModule(fragmentShaderModuleDescriptor);

        fragmentStage.shaderModule = m_fragmentShaderModule.get();

        // output targets
        fragmentStage.targets = { { .format = m_swapchain->getTextureFormat() } };
    }

    // Depth/Stencil stage
    DepthStencilStage depthStencilStage;
    {
        depthStencilStage.format = m_depthStencilTextureView->getTexture()->getFormat();
    }

    RenderPipelineDescriptor descriptor;
    descriptor.layout = m_pipelineLayout.get();
    descriptor.inputAssembly = inputAssembly;
    descriptor.vertex = vertexStage;
    descriptor.rasterization = rasterization;
    descriptor.fragment = fragmentStage;
    descriptor.depthStencil = depthStencilStage;

    m_renderPipeline = m_device->createRenderPipeline(descriptor);
}

void OBJModelSample::createCommandBuffers()
{
    CommandBufferDescriptor descriptor{ .usage = CommandBufferUsage::kOneTime };
    m_renderCommandBuffer = m_device->createCommandBuffer(descriptor);
}

void OBJModelSample::copyBufferToBuffer(Buffer* src, Buffer* dst)
{
    BlitBuffer srcBuffer{};
    srcBuffer.buffer = src;
    srcBuffer.offset = 0;

    BlitBuffer dstBuffer{};
    dstBuffer.buffer = dst;
    dstBuffer.offset = 0;

    CommandBufferDescriptor commandBufferDescriptor{ .usage = CommandBufferUsage::kOneTime };
    auto commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);

    CommandEncoderDescriptor commandEncoderDescriptor{};
    auto commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);

    commandEncoder->copyBufferToBuffer(srcBuffer, dstBuffer, src->getSize());

    m_queue->submit({ commandEncoder->finish() });
}

void OBJModelSample::copyBufferToTexture(Buffer* imageTextureStagingBuffer, Texture* imageTexture)
{
    BlitTextureBuffer blitTextureBuffer{};
    blitTextureBuffer.buffer = imageTextureStagingBuffer;
    blitTextureBuffer.offset = 0;
    uint32_t channel = 4;                          // TODO: from texture.
    uint32_t bytesPerData = sizeof(unsigned char); // TODO: from buffer.
    blitTextureBuffer.bytesPerRow = bytesPerData * imageTexture->getWidth() * channel;
    blitTextureBuffer.rowsPerTexture = imageTexture->getHeight();

    BlitTexture blitTexture{ .texture = imageTexture, .aspect = TextureAspectFlagBits::kColor };
    Extent3D extent{};
    extent.width = imageTexture->getWidth();
    extent.height = imageTexture->getHeight();
    extent.depth = 1;

    CommandBufferDescriptor commandBufferDescriptor{ .usage = CommandBufferUsage::kOneTime };
    std::unique_ptr<CommandBuffer> commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);

    CommandEncoderDescriptor commandEncoderDescriptor{};
    std::unique_ptr<CommandEncoder> commandEndoer = commandBuffer->createCommandEncoder(commandEncoderDescriptor);
    commandEndoer->copyBufferToTexture(blitTextureBuffer, blitTexture, extent);

    m_queue->submit({ commandEndoer->finish() });
}

void OBJModelSample::updateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain->getWidth() / static_cast<float>(m_swapchain->getHeight()), 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBufferMappedPointer, &ubo, sizeof(ubo));
}

} // namespace jipu

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

void android_main(struct android_app* app)
{
    jipu::SampleDescriptor descriptor{
        { 1000, 2000, "OBJModel", app },
        ""
    };

    jipu::OBJModelSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    jipu::SampleDescriptor descriptor{
        { 800, 600, "OBJModel", nullptr },
        argv[0]
    };

    jipu::OBJModelSample sample(descriptor);

    return sample.exec();
}

#endif
