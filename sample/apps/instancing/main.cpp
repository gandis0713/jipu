

#include "camera.h"
#include "file.h"
#include "im_gui.h"
#include "sample.h"

#include "vkt/gpu/buffer.h"
#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/command_encoder.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/pipeline_layout.h"
#include "vkt/gpu/queue.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace vkt
{

class InstancingSample : public Sample, public Im_Gui
{
public:
    InstancingSample() = delete;
    InstancingSample(const SampleDescriptor& descriptor);
    ~InstancingSample() override;

    void init() override;
    void update() override;
    void draw() override;

private:
    void updateImGui() override;

    void updateUniformBuffer();

private:
    void createDevier();
    void createPhysicalDevice();
    void createSurface();
    void createDevice();
    void createSwapchain();
    void createCommandBuffer();
    void createQueue();
    void createBindingGroupLayout();
    void createBindingGroup();
    void createVertexBuffer();
    void createUniformBuffer();
    void createRenderPipeline();

    void createCamera();

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;
    std::unique_ptr<BindingGroupLayout> m_bindingGroupLayout = nullptr;
    std::unique_ptr<BindingGroup> m_bindingGroup = nullptr;
    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<Buffer> m_uniformBuffer = nullptr;

    std::unique_ptr<PipelineLayout> m_renderPipelineLayout = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    struct Vertex
    {
        glm::vec3 pos;
    };

    struct MVP
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    } m_mvp;

    std::vector<Vertex> m_vertices{
        { { 0.0, -100.5, 0.0 } },
        { { -100.5, 100.5, 0.0 } },
        { { 100.5, 100.5, 0.0 } },
    };

    std::unique_ptr<Camera> m_camera = nullptr;

    uint32_t m_sampleCount = 1;
};

InstancingSample::InstancingSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

InstancingSample::~InstancingSample()
{
    clearImGui();

    m_renderPipeline.reset();
    m_renderPipelineLayout.reset();
    m_uniformBuffer.reset();
    m_vertexBuffer.reset();
    m_bindingGroup.reset();
    m_bindingGroupLayout.reset();
    m_queue.reset();
    m_commandBuffer.reset();
    m_swapchain.reset();
    m_device.reset();
    m_surface.reset();
    m_physicalDevice.reset();
    m_driver.reset();
}

void InstancingSample::init()
{

    createDevier();
    createPhysicalDevice();
    createSurface();
    createDevice();
    createSwapchain();
    createCommandBuffer();
    createQueue();
    createVertexBuffer();
    createUniformBuffer();
    createBindingGroupLayout();
    createBindingGroup();
    createRenderPipeline();

    initImGui(m_device.get(), m_queue.get(), m_swapchain.get());

    m_initialized = true;
}

void InstancingSample::updateUniformBuffer()
{
}

void InstancingSample::update()
{
    updateUniformBuffer();

    updateImGui();
    buildImGui();
}

void InstancingSample::draw()
{
    auto swapchainIndex = m_swapchain->acquireNextTexture();
    auto renderView = m_swapchain->getTextureView(swapchainIndex);
    {
        ColorAttachment attachment{};
        attachment.clearValue = { .float32 = { 0.0, 0.0, 0.0, 0.0 } };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;
        attachment.renderView = renderView;
        attachment.resolveView = nullptr;

        RenderPassEncoderDescriptor renderPassDescriptor;
        renderPassDescriptor.sampleCount = m_sampleCount;
        renderPassDescriptor.colorAttachments = { attachment };

        CommandEncoderDescriptor commandDescriptor{};
        auto commadEncoder = m_commandBuffer->createCommandEncoder(commandDescriptor);

        auto renderPassEncoder = commadEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(m_renderPipeline.get());
        renderPassEncoder->setBindingGroup(0, m_bindingGroup.get());
        renderPassEncoder->setVertexBuffer(m_vertexBuffer.get());
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->draw(static_cast<uint32_t>(m_vertices.size()));
        renderPassEncoder->end();

        drawImGui(commadEncoder.get(), renderView);

        m_queue->submit({ commadEncoder->finish() }, m_swapchain.get());
    }
}

void InstancingSample::updateImGui()
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

void InstancingSample::createDevier()
{
    DriverDescriptor descriptor{};
    descriptor.type = DriverType::VULKAN;

    m_driver = Driver::create(descriptor);
}

void InstancingSample::createPhysicalDevice()
{
    PhysicalDeviceDescriptor descriptor{};
    descriptor.index = 0; // TODO: select device.

    m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
}

void InstancingSample::createDevice()
{
    SurfaceDescriptor descriptor{};
    descriptor.windowHandle = getWindowHandle();

    m_surface = m_driver->createSurface(descriptor);
}

void InstancingSample::createSurface()
{
    DeviceDescriptor descriptor{};

    m_device = m_physicalDevice->createDevice(descriptor);
}

void InstancingSample::createSwapchain()
{
#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat textureFormat = TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
#else
    TextureFormat textureFormat = TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
#endif

    SwapchainDescriptor descriptor{};
    descriptor.width = m_width;
    descriptor.height = m_height;
    descriptor.surface = m_surface.get();
    descriptor.textureFormat = textureFormat;
    descriptor.colorSpace = ColorSpace::kSRGBNonLinear;
    descriptor.presentMode = PresentMode::kFifo;

    m_swapchain = m_device->createSwapchain(descriptor);
}

void InstancingSample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    descriptor.usage = CommandBufferUsage::kOneTime;

    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void InstancingSample::createQueue()
{
    QueueDescriptor descriptor{};
    descriptor.flags = QueueFlagBits::kGraphics;

    m_queue = m_device->createQueue(descriptor);
}

void InstancingSample::createVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_vertices.size() * sizeof(Vertex);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_vertexBuffer->map();
    memcpy(pointer, m_vertices.data(), descriptor.size);
    m_vertexBuffer->unmap();
}

void InstancingSample::createUniformBuffer()
{
    createCamera();

    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 80.0f));
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 reori = R * T;
    // m_mvp.model = reori;
    m_mvp.model = glm::mat4(1.0f);
    m_mvp.view = m_camera->getViewMat();
    m_mvp.proj = m_camera->getProjectionMat();

    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = sizeof(MVP);
    bufferDescriptor.usage = BufferUsageFlagBits::kUniform;

    m_uniformBuffer = m_device->createBuffer(bufferDescriptor);
    void* mappedPointer = m_uniformBuffer->map();
    memcpy(mappedPointer, &m_mvp, sizeof(MVP));
    // m_uniformBuffer->unmap();
}

void InstancingSample::createBindingGroupLayout()
{
    BufferBindingLayout bufferBindingLayout{};
    bufferBindingLayout.index = 0;
    bufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;
    bufferBindingLayout.type = BufferBindingType::kUniform;

    BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
    bindingGroupLayoutDescriptor.buffers = { bufferBindingLayout };

    m_bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
}

void InstancingSample::createBindingGroup()
{
    BufferBinding bufferBinding{};
    bufferBinding.index = 0;
    bufferBinding.buffer = m_uniformBuffer.get();
    bufferBinding.offset = 0;
    bufferBinding.size = m_uniformBuffer->getSize();

    BindingGroupDescriptor bindingGroupDescriptor{};
    bindingGroupDescriptor.buffers = { bufferBinding };
    bindingGroupDescriptor.layout = m_bindingGroupLayout.get();

    m_bindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
}

void InstancingSample::createRenderPipeline()
{
    // render pipeline layout
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_bindingGroupLayout.get() };

        m_renderPipelineLayout = m_device->createPipelineLayout(descriptor);
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
        std::vector<char> vertexShaderSource = utils::readFile(m_appDir / "instancing.vert.spv", m_handle);
        descriptor.code = vertexShaderSource.data();
        descriptor.codeSize = static_cast<uint32_t>(vertexShaderSource.size());

        vertexShaderModule = m_device->createShaderModule(descriptor);
    }

    // vertex stage
    VertexStage vertexStage{};
    {
        VertexAttribute positionAttribute{};
        positionAttribute.format = VertexFormat::kSFLOATx3;
        positionAttribute.offset = offsetof(Vertex, pos);

        VertexInputLayout vertexInputLayout{};
        vertexInputLayout.mode = VertexMode::kVertex;
        vertexInputLayout.stride = sizeof(Vertex);
        vertexInputLayout.attributes = { positionAttribute };

        vertexStage.entryPoint = "main";
        vertexStage.shaderModule = vertexShaderModule.get();
        vertexStage.layouts = { vertexInputLayout };
    }

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
        std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "instancing.frag.spv", m_handle);
        descriptor.code = fragmentShaderSource.data();
        descriptor.codeSize = static_cast<uint32_t>(fragmentShaderSource.size());

        fragmentShaderModule = m_device->createShaderModule(descriptor);
    }

    // fragment
    FragmentStage fragmentStage{};
    {
        FragmentStage::Target target{};
        target.format = m_swapchain->getTextureFormat();

        fragmentStage.targets = { target };
        fragmentStage.entryPoint = "main";
        fragmentStage.shaderModule = fragmentShaderModule.get();
    }

    // depth/stencil

    // render pipeline
    RenderPipelineDescriptor descriptor{};
    descriptor.inputAssembly = inputAssemblyStage;
    descriptor.vertex = vertexStage;
    descriptor.rasterization = rasterizationStage;
    descriptor.fragment = fragmentStage;
    descriptor.layout = m_renderPipelineLayout.get();

    m_renderPipeline = m_device->createRenderPipeline(descriptor);
}

void InstancingSample::createCamera()
{
    m_camera = std::make_unique<PerspectiveCamera>(glm::radians(45.0f),
                                                   m_swapchain->getWidth() / static_cast<float>(m_swapchain->getHeight()),
                                                   0.1f,
                                                   1000.0f);
    m_camera->lookAt(glm::vec3(0.0f, 0.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

} // namespace vkt

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
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Instancing", app },
        ""
    };

    vkt::InstancingSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Instancing", nullptr },
        argv[0]
    };

    vkt::InstancingSample sample(descriptor);

    return sample.exec();
}

#endif
