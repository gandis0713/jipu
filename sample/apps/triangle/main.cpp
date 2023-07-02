

#include "file.h"
#include "sample.h"
#include "vkt_headers.h"

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

using namespace vkt;

class TriangleSample : public Sample
{
public:
    TriangleSample() = delete;
    TriangleSample(const SampleDescriptor& descriptor);
    ~TriangleSample() override;

    void init() override;

private:
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffer();

    void createBindingGroupLayout();
    void createBindingGroup();

    void createPipelineLayout();
    void createRenderPipeline();
    void createCommandBuffers();

    void updateUniformBuffer(uint32_t swapImageIndex);
    void draw() override;

private:
    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;
    };

    // data
    std::vector<Vertex> m_vertices{};
    std::vector<uint16_t> m_indices{};

    // wrapper
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;

    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;

    std::unique_ptr<Queue> m_renderQueue = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;

    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<Buffer> m_indexBuffer = nullptr;

    std::vector<std::unique_ptr<Buffer>> m_uniformBuffers{};
    std::vector<void*> m_uniformBufferMappedPointers{};

    std::unique_ptr<BindingGroupLayout> m_bindingGroupLayout = nullptr;
    std::unique_ptr<BindingGroup> m_bindingGroup = nullptr;

    std::unique_ptr<PipelineLayout> m_pipelineLayout = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    std::unique_ptr<ShaderModule> m_vertexShaderModule = nullptr;
    std::unique_ptr<ShaderModule> m_fragmentShaderModule = nullptr;

    std::vector<std::unique_ptr<CommandBuffer>> m_commandBuffers{};
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

    m_commandBuffers.clear();

    m_vertexShaderModule.reset();
    m_fragmentShaderModule.reset();

    m_renderPipeline.reset();
    m_pipelineLayout.reset();

    m_bindingGroupLayout.reset();
    m_bindingGroup.reset();

    m_uniformBufferMappedPointers.clear();
    m_uniformBuffers.clear();

    m_indexBuffer.reset();
    m_vertexBuffer.reset();

    m_renderQueue.reset();

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
        QueueDescriptor descriptor{ .flags = QueueFlagBits::kGraphics };
        m_renderQueue = m_device->createQueue(descriptor);
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
    createIndexBuffer();
    createUniformBuffer();

    createBindingGroupLayout();
    createBindingGroup();

    createPipelineLayout();
    createRenderPipeline();
    createCommandBuffers();

    m_initialized = true;
}

void TriangleSample::createVertexBuffer()
{
    // vertex buffer
    m_vertices = {
        { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
    };

    uint64_t vertexSize = static_cast<uint64_t>(sizeof(Vertex) * m_vertices.size());
    BufferDescriptor vertexBufferDescriptor{ .size = vertexSize,
                                             .usage = BufferUsageFlagBits::kVertex };
    m_vertexBuffer = m_device->createBuffer(vertexBufferDescriptor);

    void* mappedPointer = m_vertexBuffer->map();
    memcpy(mappedPointer, m_vertices.data(), vertexSize);
    m_vertexBuffer->unmap();
}

void TriangleSample::createIndexBuffer()
{
    // index buffer
    m_indices = {
        0, 1, 2, 2, 3, 0
    };

    uint64_t indexSize = static_cast<uint64_t>(sizeof(uint64_t) * m_indices.size());
    BufferDescriptor indexBufferDescriptor{ .size = indexSize,
                                            .usage = BufferUsageFlagBits::kIndex };

    m_indexBuffer = m_device->createBuffer(indexBufferDescriptor);

    void* mappedPointer = m_indexBuffer->map();
    memcpy(mappedPointer, m_indices.data(), indexSize);
    m_indexBuffer->unmap();
}

void TriangleSample::createUniformBuffer()
{
    const uint32_t swapImageCount = m_swapchain->getTextureViews().size();
    m_uniformBuffers.resize(swapImageCount);
    m_uniformBufferMappedPointers.resize(swapImageCount);
    for (uint32_t i = 0; i < swapImageCount; ++i)
    {
        BufferDescriptor descriptor{ .size = sizeof(UniformBufferObject),
                                     .usage = BufferUsageFlagBits::kUniform };
        auto buffer = m_device->createBuffer(descriptor);

        m_uniformBufferMappedPointers[i] = buffer->map();
        m_uniformBuffers[i] = std::move(buffer);
    }
}

void TriangleSample::createBindingGroupLayout()
{
    BufferBindingLayout bufferBindingLayout{ .type = BufferBindingType::kUniform };
    // BufferBindingGroupLayoutEntry bufferBindingGroupLayoutEntry{ { .index = 0,
    //                                                                .stages = BindingStageFlagBits::kVertexStage },
    //                                                              .layout = bufferBindingLayout };
    BufferBindingGroupLayoutEntry bufferBindingGroupLayoutEntry{};
    bufferBindingGroupLayoutEntry.index = 0;
    bufferBindingGroupLayoutEntry.stages = BindingStageFlagBits::kVertexStage;
    bufferBindingGroupLayoutEntry.layout = bufferBindingLayout;

    std::vector<BufferBindingGroupLayoutEntry> bufferLayoutEntries{ bufferBindingGroupLayoutEntry };
    BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{ .buffers = bufferLayoutEntries };

    m_bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
}

void TriangleSample::createBindingGroup()
{
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
                vertexAttributes[0] = { .format = VertexFormat::kSFLOATx2,
                                        .offset = offsetof(Vertex, pos) };

                // color
                vertexAttributes[1] = { .format = VertexFormat::kSFLOATx3,
                                        .offset = offsetof(Vertex, color) };
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

    RenderPipelineDescriptor descriptor{ { .layout = m_pipelineLayout.get() },
                                         .inputAssembly = inputAssembly,
                                         .vertex = vertexStage,
                                         .rasterization = rasterization,
                                         .fragment = fragmentStage };

    m_renderPipeline = m_device->createRenderPipeline(descriptor);
}

void TriangleSample::createCommandBuffers()
{
    std::vector<TextureView*> swapchainTextureViews = m_swapchain->getTextureViews();

    auto commandBufferCount = swapchainTextureViews.size();
    m_commandBuffers.resize(commandBufferCount);
    for (auto i = 0; i < commandBufferCount; ++i)
    {
        CommandBufferDescriptor descriptor{};
        auto commandBuffer = m_device->createCommandBuffer(descriptor);
        m_commandBuffers[i] = std::move(commandBuffer);
    }

    for (auto i = 0; i < commandBufferCount; ++i)
    {
        auto commandBuffer = m_commandBuffers[i].get();

        std::vector<ColorAttachment> colorAttachments(1); // in currently. use only one.
        colorAttachments[0] = { .textureView = swapchainTextureViews[i],
                                .loadOp = LoadOp::kClear,
                                .storeOp = StoreOp::kStore,
                                .clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } } };
        DepthStencilAttachment depthStencilAttachment{};

        RenderCommandEncoderDescriptor descriptor{ .colorAttachments = colorAttachments,
                                                   .depthStencilAttachment = depthStencilAttachment };
        auto renderCommandEncoder = commandBuffer->createRenderCommandEncoder(descriptor);
        m_renderCommandEncoder.push_back(std::move(renderCommandEncoder));
    }
}

void TriangleSample::updateUniformBuffer(uint32_t swapImageIndex)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain->getWidth() / static_cast<float>(m_swapchain->getHeight()), 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBufferMappedPointers[swapImageIndex], &ubo, sizeof(ubo));
}

void TriangleSample::draw()
{

    int nextImageIndex = m_swapchain->acquireNextTexture();
    updateUniformBuffer(nextImageIndex);

    auto renderCommandEncoder = m_renderCommandEncoder[nextImageIndex].get();

    renderCommandEncoder->begin();
    renderCommandEncoder->setPipeline(m_renderPipeline.get());
    renderCommandEncoder->setVertexBuffer(m_vertexBuffer.get());
    renderCommandEncoder->setIndexBuffer(m_indexBuffer.get());
    renderCommandEncoder->setViewport(0, 0, m_width, m_height, 0, 1); // set viewport state.
    renderCommandEncoder->setScissor(0, 0, m_width, m_height);        // set scissor state.
    renderCommandEncoder->drawIndexed(static_cast<uint32_t>(m_indices.size()));
    renderCommandEncoder->end();

    m_renderQueue->submit(renderCommandEncoder->getCommandBuffer());
    m_swapchain->present(m_renderQueue.get());
}

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    SampleDescriptor descriptor{
        { 1000, 2000, "Triangle", app },
        ""
    };

    TriangleSample triangleSample(descriptor);

    triangleSample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    SampleDescriptor descriptor{
        { 800, 600, "Triangle", nullptr },
        argv[0]
    };

    TriangleSample triangleSample(descriptor);

    return triangleSample.exec();
}

#endif
