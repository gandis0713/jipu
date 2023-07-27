

#include "file.h"
#include "jpeg.h"
#include "model.h"
#include "sample.h"

#include "vkt/gpu/binding_group.h"
#include "vkt/gpu/binding_group_layout.h"
#include "vkt/gpu/buffer.h"
#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/pipeline_layout.h"
#include "vkt/gpu/queue.h"
#include "vkt/gpu/sampler.h"
#include "vkt/gpu/shader_module.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"
#include "vkt/gpu/texture_view.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <spdlog/spdlog.h>
#include <stddef.h>

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

class TriangleSample : public Sample
{
public:
    TriangleSample() = delete;
    TriangleSample(const SampleDescriptor& descriptor);
    ~TriangleSample() override;

    void init() override;

private:
    void createVertexBuffer();
    void createUniformBuffer();

    void createImageTexture();
    void createImageTextureView();
    void createImageSampler();

    void createDepthStencilTexture();
    void createDepthStencilTextureView();

    void createBindingGroupLayout();
    void createBindingGroup();

    void createPipelineLayout();
    void createRenderPipeline();
    void createCommandBuffers();

    void copyBufferToBuffer();
    void copyBufferToTexture(Buffer* imageTextureBuffer, Texture* imageTexture);

    void updateUniformBuffer();
    void draw() override;

private:
    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    // data
    Polygon m_polygon{};
    std::unique_ptr<JPEGImage> m_jpegImage = nullptr;

    // wrapper
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;

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

    std::unique_ptr<Texture> m_depthStencilTexture = nullptr;
    std::unique_ptr<TextureView> m_depthStencilTextureView = nullptr;

    std::unique_ptr<BindingGroupLayout> m_bindingGroupLayout = nullptr;
    std::unique_ptr<BindingGroup> m_bindingGroup = nullptr;

    std::unique_ptr<PipelineLayout> m_pipelineLayout = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    std::unique_ptr<ShaderModule> m_vertexShaderModule = nullptr;
    std::unique_ptr<ShaderModule> m_fragmentShaderModule = nullptr;

    std::vector<std::unique_ptr<CommandBuffer>> m_renderCommandBuffers{};
    std::vector<std::unique_ptr<RenderCommandEncoder>> m_renderCommandEncoder{};
};

TriangleSample::TriangleSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

TriangleSample::~TriangleSample()
{
    // clear swapchain first.
    m_swapchain.reset();

    m_renderCommandBuffers.clear();

    m_vertexShaderModule.reset();
    m_fragmentShaderModule.reset();

    m_renderPipeline.reset();
    m_pipelineLayout.reset();

    m_bindingGroupLayout.reset();
    m_bindingGroup.reset();

    m_depthStencilTextureView.reset();
    m_depthStencilTexture.reset();

    // unmap m_uniformBufferMappedPointer;
    m_uniformBuffer.reset();

    m_imageSampler.reset();
    m_imageTextureView.reset();
    m_imageTexture.reset();

    m_indexBuffer.reset();
    m_vertexBuffer.reset();

    m_queue.reset();

    m_physicalDevice.reset();
    m_device.reset();

    m_surface.reset();
    m_driver.reset();
}

void TriangleSample::init()
{
    // create Driver.
    {
        DriverDescriptor descriptor{ .type = DriverType::VULKAN };
        m_driver = Driver::create(descriptor);
    }

    // create surface
    {
        SurfaceDescriptor descriptor{ .windowHandle = getWindowHandle() };
        m_surface = m_driver->createSurface(descriptor);
    }

    // create PhysicalDevice.
    {
        PhysicalDeviceDescriptor descriptor{};
        m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
    }

    // create Device.
    {
        DeviceDescriptor descriptor{};
        m_device = m_physicalDevice->createDevice(descriptor);
    }

    // create queue
    {
        QueueDescriptor rednerQueueDescriptor{ .flags = QueueFlagBits::kGraphics | QueueFlagBits::kTransfer };
        m_queue = m_device->createQueue(rednerQueueDescriptor);
    }

    // create swapchain
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

    // create buffer
    createVertexBuffer();
    createUniformBuffer();

    createImageTexture();
    createImageTextureView();
    createImageSampler();

    createDepthStencilTexture();
    createDepthStencilTextureView();

    createBindingGroupLayout();
    createBindingGroup();

    createPipelineLayout();
    createRenderPipeline();
    createCommandBuffers();

    m_initialized = true;
}

void TriangleSample::createVertexBuffer()
{
    std::filesystem::path objPath = m_path.parent_path() / "viking_room.obj";
    m_polygon = loadOBJ(objPath);

    // vertex buffer
    {
        uint64_t vertexSize = static_cast<uint64_t>(sizeof(Vertex) * m_polygon.vertices.size());
        BufferDescriptor vertexBufferDescriptor{ .size = vertexSize,
                                                 .usage = BufferUsageFlagBits::kVertex };
        m_vertexBuffer = m_device->createBuffer(vertexBufferDescriptor);

        void* mappedPointer = m_vertexBuffer->map();
        memcpy(mappedPointer, m_polygon.vertices.data(), vertexSize);
        m_vertexBuffer->unmap();
    }

    // index buffer
    {
        uint64_t indexSize = static_cast<uint64_t>(sizeof(uint16_t) * m_polygon.indices.size());
        BufferDescriptor indexBufferDescriptor{ .size = indexSize,
                                                .usage = BufferUsageFlagBits::kIndex };

        m_indexBuffer = m_device->createBuffer(indexBufferDescriptor);

        void* mappedPointer = m_indexBuffer->map();
        memcpy(mappedPointer, m_polygon.indices.data(), indexSize);
        m_indexBuffer->unmap();
    }
}

void TriangleSample::createUniformBuffer()
{
    BufferDescriptor descriptor{ .size = sizeof(UniformBufferObject),
                                 .usage = BufferUsageFlagBits::kUniform };
    m_uniformBuffer = m_device->createBuffer(descriptor);
    m_uniformBufferMappedPointer = m_uniformBuffer->map();
}

void TriangleSample::createImageTexture()
{
    // load jpeg image.
    m_jpegImage = std::make_unique<JPEGImage>(m_path.parent_path() / "viking_room.png");

    unsigned char* pixels = static_cast<unsigned char*>(m_jpegImage->getPixels());
    uint32_t width = m_jpegImage->getWidth();
    uint32_t height = m_jpegImage->getHeight();
    uint32_t channel = m_jpegImage->getChannel();
    uint64_t imageSize = sizeof(unsigned char) * width * height * channel;

    // create image staging buffer.
    BufferDescriptor descriptor{ .size = imageSize, .usage = BufferUsageFlagBits::kCopySrc };
    std::unique_ptr<Buffer> imageTextureStagingBuffer = m_device->createBuffer(descriptor);

    void* mappedPointer = imageTextureStagingBuffer->map();
    memcpy(mappedPointer, pixels, imageSize);
    // m_imageStagingBuffer->unmap();

    // create texture.
    TextureDescriptor textureDescriptor{ .type = TextureType::k2D,
                                         .format = TextureFormat::kRGBA_8888_UInt_Norm_SRGB,
                                         .usages = TextureUsageFlagBits::kCopyDst | TextureUsageFlagBits::kTextureBinding,
                                         .width = width,
                                         .height = height };
    m_imageTexture = m_device->createTexture(textureDescriptor);

    // copy image staging buffer to texture
    copyBufferToTexture(imageTextureStagingBuffer.get(), m_imageTexture.get());
}

void TriangleSample::createImageTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_imageTextureView = m_imageTexture->createTextureView(descriptor);
}

void TriangleSample::createDepthStencilTexture()
{
    TextureDescriptor descriptor{};
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kD_32_SFloat;
    descriptor.usages = TextureUsageFlagBits::kDepthStencil;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();

    m_depthStencilTexture = m_device->createTexture(descriptor);
}

void TriangleSample::createDepthStencilTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kDepth;

    m_depthStencilTextureView = m_depthStencilTexture->createTextureView(descriptor);
}

void TriangleSample::createImageSampler()
{
    SamplerDescriptor descriptor{};
    descriptor.magFilter = FilterMode::kLinear;
    descriptor.minFilter = FilterMode::kLinear;
    descriptor.mipmapFilter = MipmapFilterMode::kLinear;
    descriptor.addressModeU = AddressMode::kRepeat;
    descriptor.addressModeV = AddressMode::kRepeat;
    descriptor.addressModeW = AddressMode::kRepeat;

    m_imageSampler = m_device->createSampler(descriptor);
}

void TriangleSample::createBindingGroupLayout()
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
    std::vector<SamplerBindingLayout> samplerBindingLayouts{ samplerBindingLayout };

    BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{ .buffers = bufferBindingLayouts,
                                                               .samplers = samplerBindingLayouts };

    m_bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
}

void TriangleSample::createBindingGroup()
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

void TriangleSample::createPipelineLayout()
{
    PipelineLayoutDescriptor pipelineLayoutDescriptor{ .layouts = { m_bindingGroupLayout.get() } };
    m_pipelineLayout = m_device->createPipelineLayout(pipelineLayoutDescriptor);
}

void TriangleSample::createRenderPipeline()
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
        auto appDir = m_path.parent_path();
        const std::vector<char> vertShaderCode = utils::readFile(appDir / "triangle.vert.spv", m_handle);
        ShaderModuleDescriptor vertexShaderModuleDescriptor{ .code = vertShaderCode.data(),
                                                             .codeSize = vertShaderCode.size() };
        m_vertexShaderModule = m_device->createShaderModule(vertexShaderModuleDescriptor);
        vertexStage.shader = m_vertexShaderModule.get();

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
                                        .offset = offsetof(Vertex, pos) };

                // texture coodinate
                vertexAttributes[1] = { .format = VertexFormat::kSFLOATx2,
                                        .offset = offsetof(Vertex, texCoord) };
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
    }

    // fragment stage
    FragmentStage fragmentStage{};
    {
        // create fragment shader
        auto appDir = m_path.parent_path();
        const std::vector<char> fragShaderCode = utils::readFile(appDir / "triangle.frag.spv", m_handle);
        ShaderModuleDescriptor fragmentShaderModuleDescriptor{ .code = fragShaderCode.data(),
                                                               .codeSize = fragShaderCode.size() };
        m_fragmentShaderModule = m_device->createShaderModule(fragmentShaderModuleDescriptor);

        fragmentStage.shader = m_fragmentShaderModule.get();

        // output targets
        fragmentStage.targets = { { .format = m_swapchain->getTextureFormat() } };
    }

    RenderPipelineDescriptor descriptor;
    descriptor.layout = m_pipelineLayout.get();
    descriptor.inputAssembly = inputAssembly;
    descriptor.vertex = vertexStage;
    descriptor.rasterization = rasterization;
    descriptor.fragment = fragmentStage;

    m_renderPipeline = m_device->createRenderPipeline(descriptor);
}

void TriangleSample::createCommandBuffers()
{
    std::vector<TextureView*> swapchainTextureViews = m_swapchain->getTextureViews();

    auto commandBufferCount = swapchainTextureViews.size();
    m_renderCommandBuffers.resize(commandBufferCount);
    for (auto i = 0; i < commandBufferCount; ++i)
    {
        CommandBufferDescriptor descriptor{ .usage = CommandBufferUsage::kUndefined };
        auto commandBuffer = m_device->createCommandBuffer(descriptor);
        m_renderCommandBuffers[i] = std::move(commandBuffer);
    }

    for (auto i = 0; i < commandBufferCount; ++i)
    {
        auto commandBuffer = m_renderCommandBuffers[i].get();

        std::vector<ColorAttachment> colorAttachments(1); // in currently. use only one.
        colorAttachments[0] = { .textureView = swapchainTextureViews[i],
                                .loadOp = LoadOp::kClear,
                                .storeOp = StoreOp::kStore,
                                .clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } } };
        DepthStencilAttachment depthStencilAttachment{ .textureView = m_depthStencilTextureView.get(),
                                                       .loadOp = LoadOp::kClear,
                                                       .storeOp = StoreOp::kStore,
                                                       .clearValue = { .depth = 1.0f, .stencil = 0 } };

        RenderCommandEncoderDescriptor descriptor{ .colorAttachments = colorAttachments,
                                                   .depthStencilAttachment = depthStencilAttachment };
        auto renderCommandEncoder = commandBuffer->createRenderCommandEncoder(descriptor);
        m_renderCommandEncoder.push_back(std::move(renderCommandEncoder));
    }
}

void TriangleSample::copyBufferToBuffer()
{
    // TODO
}

void TriangleSample::copyBufferToTexture(Buffer* imageTextureStagingBuffer, Texture* imageTexture)
{
    CommandBufferDescriptor commandBufferDescriptor{ .usage = CommandBufferUsage::kOneTime };
    std::unique_ptr<CommandBuffer> blitCommandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);

    BlitCommandEncoderDescriptor blitCommandEncoderDescriptor{};
    std::unique_ptr<BlitCommandEncoder> blitCommandEncoder = blitCommandBuffer->createBlitCommandEncoder(blitCommandEncoderDescriptor);

    BlitTextureBuffer blitTextureBuffer{};
    blitTextureBuffer.buffer = imageTextureStagingBuffer;
    blitTextureBuffer.offset = 0;
    uint32_t channel = 4;                          // TODO: from texture.
    uint32_t bytesPerData = sizeof(unsigned char); // TODO: from buffer.
    blitTextureBuffer.bytesPerRow = bytesPerData * imageTexture->getWidth() * channel;
    blitTextureBuffer.rowsPerTexture = imageTexture->getHeight();

    BlitTexture blitTexture{ .texture = imageTexture };
    Extent3D extent{};
    extent.width = imageTexture->getWidth();
    extent.height = imageTexture->getHeight();
    extent.depth = 1;

    blitCommandEncoder->begin();
    blitCommandEncoder->copyBufferToTexture(blitTextureBuffer, blitTexture, extent);
    blitCommandEncoder->end();

    m_queue->submit({ blitCommandEncoder->getCommandBuffer() });
}

void TriangleSample::updateUniformBuffer()
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

void TriangleSample::draw()
{
    updateUniformBuffer();

    int nextImageIndex = m_swapchain->acquireNextTexture();

    auto renderCommandEncoder = m_renderCommandEncoder[nextImageIndex].get();

    renderCommandEncoder->begin();
    renderCommandEncoder->setPipeline(m_renderPipeline.get());
    renderCommandEncoder->setBindingGroup(0, m_bindingGroup.get());
    renderCommandEncoder->setVertexBuffer(m_vertexBuffer.get());
    renderCommandEncoder->setIndexBuffer(m_indexBuffer.get());
    renderCommandEncoder->setViewport(0, 0, m_width, m_height, 0, 1); // set viewport state.
    renderCommandEncoder->setScissor(0, 0, m_width, m_height);        // set scissor state.
    renderCommandEncoder->drawIndexed(static_cast<uint32_t>(m_polygon.indices.size()));
    renderCommandEncoder->end();

    m_queue->submit({ renderCommandEncoder->getCommandBuffer() }, m_swapchain.get());
}

} // namespace vkt

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Triangle", app },
        ""
    };

    vkt::TriangleSample triangleSample(descriptor);

    triangleSample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Triangle", nullptr },
        argv[0]
    };

    vkt::TriangleSample triangleSample(descriptor);

    return triangleSample.exec();
}

#endif
