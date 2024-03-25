

#include "file.h"
#include "im_gui.h"
#include "sample.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace jipu
{

class ImGuiSample : public Sample, public Im_Gui
{
public:
    ImGuiSample() = delete;
    ImGuiSample(const SampleDescriptor& descriptor);
    ~ImGuiSample() override;

    void init() override;
    void update() override;
    void draw() override;

private:
    void updateImGui() override;

private:
    void createDevier();
    void getPhysicalDevices();
    void createSurface();
    void createDevice();
    void createSwapchain();
    void createCommandBuffer();
    void createQueue();
    void createVertexBuffer();
    void createRenderPipeline();

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::vector<std::unique_ptr<PhysicalDevice>> m_physicalDevices{};
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;
    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
    };

    std::vector<Vertex> m_vertices{
        { { 0.0, -0.5, 0.0 }, { 1.0, 0.0, 0.0 } },
        { { -0.5, 0.5, 0.0 }, { 0.0, 1.0, 0.0 } },
        { { 0.5, 0.5, 0.0 }, { 0.0, 0.0, 1.0 } },
    };

    uint32_t m_sampleCount = 1;
};

ImGuiSample::ImGuiSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

ImGuiSample::~ImGuiSample()
{
    clearImGui();

    m_renderPipeline.reset();
    m_vertexBuffer.reset();
    m_queue.reset();
    m_commandBuffer.reset();
    m_swapchain.reset();
    m_device.reset();
    m_surface.reset();
    m_physicalDevices.clear();
    m_driver.reset();
}

void ImGuiSample::init()
{
    createDevier();
    getPhysicalDevices();
    createSurface();
    createDevice();
    createSwapchain();
    createCommandBuffer();
    createQueue();
    createVertexBuffer();
    createRenderPipeline();

    initImGui(m_device.get(), m_queue.get(), *m_swapchain);

    m_initialized = true;
}

void ImGuiSample::update()
{
    updateImGui();
    buildImGui();
}

void ImGuiSample::draw()
{
    auto& renderView = m_swapchain->acquireNextTexture();
    {
        ColorAttachment attachment{
            .renderView = renderView
        };
        attachment.clearValue = { .float32 = { 0.0, 0.0, 0.0, 0.0 } };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;

        RenderPassEncoderDescriptor renderPassDescriptor{
            .colorAttachments = { attachment },
            .sampleCount = m_sampleCount
        };

        CommandEncoderDescriptor commandDescriptor{};
        auto commandEncoder = m_commandBuffer->createCommandEncoder(commandDescriptor);

        auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassDescriptor);

        renderPassEncoder->setPipeline(*m_renderPipeline);
        renderPassEncoder->setVertexBuffer(0, *m_vertexBuffer);
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->draw(static_cast<uint32_t>(m_vertices.size()));
        renderPassEncoder->end();

        drawImGui(commandEncoder.get(), renderView);

        m_queue->submit({ commandEncoder->finish() }, *m_swapchain);
    }
}

void ImGuiSample::updateImGui()
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
    debugWindow();
    ImGui::Render();
}

void ImGuiSample::createDevier()
{
    DriverDescriptor descriptor{};
    descriptor.type = DriverType::kVulkan;

    m_driver = Driver::create(descriptor);
}

void ImGuiSample::getPhysicalDevices()
{
    m_physicalDevices = m_driver->getPhysicalDevices();
}

void ImGuiSample::createDevice()
{
    // TODO: select suit device.
    PhysicalDevice* physicalDevice = m_physicalDevices[0].get();

    DeviceDescriptor descriptor;
    m_device = physicalDevice->createDevice(descriptor);
}

void ImGuiSample::createSurface()
{
    SurfaceDescriptor descriptor{};
    descriptor.windowHandle = getWindowHandle();

    m_surface = m_driver->createSurface(descriptor);
}

void ImGuiSample::createSwapchain()
{
#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat textureFormat = TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
#else
    TextureFormat textureFormat = TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
#endif

    SwapchainDescriptor descriptor{
        .surface = *m_surface,
        .textureFormat = textureFormat,
        .presentMode = PresentMode::kFifo,
        .colorSpace = ColorSpace::kSRGBNonLinear,
        .width = m_width,
        .height = m_height
    };

    m_swapchain = m_device->createSwapchain(descriptor);
}

void ImGuiSample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    descriptor.usage = CommandBufferUsage::kOneTime;

    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void ImGuiSample::createQueue()
{
    QueueDescriptor descriptor{};
    descriptor.flags = QueueFlagBits::kGraphics;

    m_queue = m_device->createQueue(descriptor);
}

void ImGuiSample::createVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_vertices.size() * sizeof(Vertex);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_vertexBuffer->map();
    memcpy(pointer, m_vertices.data(), descriptor.size);
    m_vertexBuffer->unmap();
}

void ImGuiSample::createRenderPipeline()
{
    // render pipeline layout
    std::unique_ptr<PipelineLayout> renderPipelineLayout = nullptr;
    {
        PipelineLayoutDescriptor descriptor{};

        renderPipelineLayout = m_device->createPipelineLayout(descriptor);
    }

    // input assembly stage
    InputAssemblyStage inputAssemblyStage{};
    {
        inputAssemblyStage.topology = PrimitiveTopology::kTriangleList;
    }

    // vertex shader module
    std::unique_ptr<ShaderModule> vertexShaderModule = nullptr;
    {
        ShaderModuleDescriptor descriptor{};
        std::vector<char> vertexShaderSource = utils::readFile(m_appDir / "triangle.vert.spv", m_handle);
        descriptor.code = vertexShaderSource.data();
        descriptor.codeSize = static_cast<uint32_t>(vertexShaderSource.size());

        vertexShaderModule = m_device->createShaderModule(descriptor);
    }

    // vertex stage

    VertexAttribute positionAttribute{};
    positionAttribute.format = VertexFormat::kSFLOATx3;
    positionAttribute.offset = offsetof(Vertex, pos);
    positionAttribute.location = 0;

    VertexAttribute colorAttribute{};
    colorAttribute.format = VertexFormat::kSFLOATx3;
    colorAttribute.offset = offsetof(Vertex, color);
    colorAttribute.location = 1;

    VertexInputLayout vertexInputLayout{};
    vertexInputLayout.mode = VertexMode::kVertex;
    vertexInputLayout.stride = sizeof(Vertex);
    vertexInputLayout.attributes = { positionAttribute, colorAttribute };

    VertexStage vertexStage{
        { *vertexShaderModule, "main" },
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
        ShaderModuleDescriptor descriptor{};
        std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "triangle.frag.spv", m_handle);
        descriptor.code = fragmentShaderSource.data();
        descriptor.codeSize = static_cast<uint32_t>(fragmentShaderSource.size());

        fragmentShaderModule = m_device->createShaderModule(descriptor);
    }

    // fragment

    FragmentStage::Target target{};
    target.format = m_swapchain->getTextureFormat();

    FragmentStage fragmentStage{
        { *fragmentShaderModule, "main" },
        { target }
    };

    // depth/stencil

    // render pipeline
    RenderPipelineDescriptor descriptor{
        { *renderPipelineLayout },
        inputAssemblyStage,
        vertexStage,
        rasterizationStage,
        fragmentStage
    };

    m_renderPipeline = m_device->createRenderPipeline(descriptor);
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
        { 1000, 2000, "ImGui", app },
        ""
    };

    jipu::ImGuiSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    jipu::SampleDescriptor descriptor{
        { 800, 600, "ImGui", nullptr },
        argv[0]
    };

    jipu::ImGuiSample sample(descriptor);

    return sample.exec();
}

#endif
