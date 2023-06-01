

#include "file.h"
#include "sample.h"
#include "vkt_headers.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <stddef.h>

using namespace vkt;

class TriangleSample : public Sample
{
public:
    TriangleSample() = delete;
    TriangleSample(int width, int height, const std::string& title, const char* path);
    ~TriangleSample() override;

private:
    void createRenderPipeline();
    void createCommandBuffers();

    void draw() override;

private:
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

    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    std::unique_ptr<ShaderModule> m_vertexShaderModule = nullptr;
    std::unique_ptr<ShaderModule> m_fragmentShaderModule = nullptr;

    std::vector<std::unique_ptr<CommandBuffer>> m_commandBuffers{};
};

TriangleSample::TriangleSample(int width, int height, const std::string& title, const char* path)
    : Sample(width, height, title, path)
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
        SwapchainDescriptor descriptor{ .textureFormat = TextureFormat::kBGRA_8888_UInt_Norm,
                                        .presentMode = PresentMode::kFifo,
                                        .colorSpace = ColorSpace::kSRGBNonLinear,
                                        .width = 800,
                                        .height = 600,
                                        .surface = m_surface.get() };
        m_swapchain = m_device->createSwapchain(descriptor);
    }

    // create buffer
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

        // index buffer
        m_indices = {
            0, 1, 2, 2, 3, 0
        };

        uint64_t indexSize = static_cast<uint64_t>(sizeof(uint64_t) * m_indices.size());
        BufferDescriptor indexBufferDescriptor{ .size = indexSize,
                                                .usage = BufferUsageFlagBits::kIndex };

        m_indexBuffer = m_device->createBuffer(indexBufferDescriptor);

        mappedPointer = m_indexBuffer->map();
        memcpy(mappedPointer, m_indices.data(), indexSize);
        m_indexBuffer->unmap();
    }

    createRenderPipeline();
    createCommandBuffers();
}

TriangleSample::~TriangleSample()
{
    m_commandBuffers.clear();

    m_vertexShaderModule.reset();
    m_fragmentShaderModule.reset();

    m_renderPipeline.reset();

    m_indexBuffer.reset();
    m_vertexBuffer.reset();

    m_swapchain.reset();
    m_renderQueue.reset();

    m_physicalDevice.reset();
    m_device.reset();

    m_surface.reset();
    m_driver.reset();
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
        const std::vector<char> vertShaderCode = utils::readFile(appDir / "triangle_vert.spv");
        ShaderModuleDescriptor vertexShaderModuleDescriptor{ .code = vertShaderCode.data(),
                                                             .codeSize = vertShaderCode.size() };
        m_vertexShaderModule = m_device->createShaderModule(vertexShaderModuleDescriptor);
        vertexStage.shader = m_vertexShaderModule.get();

        // layouts
        std::vector<VertexBindingLayout> layouts{};
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

            VertexBindingLayout vertexLayout{ .mode = VertexMode::kVertex,
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
        const std::vector<char> fragShaderCode = utils::readFile(appDir / "triangle_frag.spv");
        ShaderModuleDescriptor fragmentShaderModuleDescriptor{ .code = fragShaderCode.data(),
                                                               .codeSize = fragShaderCode.size() };
        m_fragmentShaderModule = m_device->createShaderModule(fragmentShaderModuleDescriptor);

        fragmentStage.shader = m_fragmentShaderModule.get();

        // output targets
        fragmentStage.targets = { { .format = m_swapchain->getTextureFormat() } };
    }

    RenderPipelineDescriptor descriptor{ .inputAssembly = inputAssembly,
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
        renderCommandEncoder->begin();
        renderCommandEncoder->setPipeline(m_renderPipeline.get());
        renderCommandEncoder->setVertexBuffer(m_vertexBuffer.get());
        renderCommandEncoder->setIndexBuffer(m_indexBuffer.get());
        renderCommandEncoder->drawIndexed(static_cast<uint32_t>(m_indices.size()));
        renderCommandEncoder->end();
    }
}

void TriangleSample::draw()
{
    int nextImageIndex = m_swapchain->acquireNextTexture();
    m_renderQueue->submit(m_commandBuffers[nextImageIndex].get());
    m_swapchain->present(m_renderQueue.get());
}

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    TriangleSample triangleSample(800, 600, "Triangle", argv[0]);

    return triangleSample.exec();
}
