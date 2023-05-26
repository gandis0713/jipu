

#include "file.h"
#include "sample.h"
#include "vkt_headers.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

using namespace vkt;

class TriangleSample : public Sample
{
public:
    TriangleSample() = default;
    TriangleSample(int argc, char** argv);
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

    // wrapper
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;

    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;

    std::unique_ptr<Queue> m_renderQueue = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;

    std::unique_ptr<Buffer> m_buffer = nullptr;

    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    std::unique_ptr<ShaderModule> m_vertexShaderModule = nullptr;
    std::unique_ptr<ShaderModule> m_fragmentShaderModule = nullptr;

    std::vector<std::unique_ptr<CommandBuffer>> m_commandBuffers{};
};

TriangleSample::TriangleSample(int argc, char** argv)
    : Sample(argc, argv)
{

    // create Driver.
    {
        DriverDescriptor descriptor{ .type = DRIVER_TYPE::VULKAN };
        m_driver = Driver::create(descriptor);
    }

    // create surface
    {
        SurfaceDescriptor descriptor{ .windowHandle = getNativeWindow() };
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
        m_vertices = {
            { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
            { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
        };

        uint64_t size = static_cast<uint64_t>(sizeof(Vertex) * m_vertices.size());
        BufferDescriptor bufferDescriptor{ .size = size,
                                           .flags = BufferFlagBits::kVertex };
        m_buffer = m_device->createBuffer(bufferDescriptor);

        void* mappedPointer = m_buffer->map();
        memcpy(mappedPointer, m_vertices.data(), size);
        m_buffer->unmap();
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

    m_buffer.reset();

    m_swapchain.reset();
    m_renderQueue.reset();

    m_physicalDevice.reset();
    m_device.reset();

    m_surface.reset();
    m_driver.reset();
}

void TriangleSample::createRenderPipeline()
{
    // vertex shader
    const std::vector<char> vertShaderCode = utils::readFile((TriangleSample::getDir() / "triangle_vert.spv"));
    ShaderModuleDescriptor vertexShaderModuleDescriptor{ .code = vertShaderCode.data(),
                                                         .codeSize = vertShaderCode.size() };
    m_vertexShaderModule = m_device->createShaderModule(vertexShaderModuleDescriptor);

    // fragment shader
    const std::vector<char> fragShaderCode = utils::readFile((TriangleSample::getDir() / "triangle_frag.spv"));
    ShaderModuleDescriptor fragmentShaderModuleDescriptor{ .code = fragShaderCode.data(),
                                                           .codeSize = fragShaderCode.size() };
    m_fragmentShaderModule = m_device->createShaderModule(fragmentShaderModuleDescriptor);

    // vertex stage
    VertexStage vertexStage{};
    vertexStage.shader = m_vertexShaderModule.get();

    // fragment stage
    FragmentStage fragmentStage{};
    fragmentStage.shader = m_fragmentShaderModule.get();
    fragmentStage.targets = { { .format = m_swapchain->getTextureFormat() } };

    RenderPipelineDescriptor descriptor{ .vertex = vertexStage,
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
        renderCommandEncoder->setVertexBuffer(m_buffer.get());
        renderCommandEncoder->draw(static_cast<uint32_t>(m_vertices.size()));
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

    TriangleSample triangleSample(argc, argv);

    return triangleSample.exec();
}
