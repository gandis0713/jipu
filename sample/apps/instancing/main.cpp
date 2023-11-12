

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

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <spdlog/spdlog.h>

static const uint32_t VERTEX_SLOT = 0;
static const uint32_t INSTANCING_SLOT = 1;

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
    void createVertexBuffer();
    void createInstancingBuffer();
    void createIndexBuffer();

    void createInstancingUniformBuffer();
    void createInstancingBindingGroupLayout();
    void createInstancingBindingGroup();
    void createInstancingRenderPipeline();

    void createNonInstancingUniformBuffer();
    void createNonInstancingBindingGroupLayout();
    void createNonInstancingBindingGroup();
    void createNonInstancingRenderPipeline();

    void createCamera();

private:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
    };

    struct MVP
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct Instancing
    {
        glm::vec3 shift;
    };

    struct Cube
    {
        Cube(float len, glm::vec3 pos = glm::vec3(0.0f))
        {
            vertices[0] = { { -len + pos[0], -len + pos[1], -len + pos[2] }, { 1.0, 0.0, 0.0 } };
            vertices[1] = { { +len + pos[0], -len + pos[1], -len + pos[2] }, { 1.0, 1.0, 0.0 } };
            vertices[2] = { { +len + pos[0], +len + pos[1], -len + pos[2] }, { 1.0, 0.0, 1.0 } };
            vertices[3] = { { -len + pos[0], +len + pos[1], -len + pos[2] }, { 1.0, 1.0, 1.0 } };
            vertices[4] = { { -len + pos[0], -len + pos[1], +len + pos[2] }, { 0.0, 1.0, 1.0 } };
            vertices[5] = { { +len + pos[0], -len + pos[1], +len + pos[2] }, { 0.0, 1.0, 0.0 } };
            vertices[6] = { { +len + pos[0], +len + pos[1], +len + pos[2] }, { 0.0, 0.0, 1.0 } };
            vertices[7] = { { -len + pos[0], +len + pos[1], +len + pos[2] }, { 1.0, 0.5, 0.5 } };
        }
        Vertex vertices[8];
    };

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;
    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<Buffer> m_instancingBuffer = nullptr;
    std::unique_ptr<Buffer> m_indexBuffer = nullptr;

    struct
    {
        std::unique_ptr<BindingGroupLayout> bindingGroupLayout = nullptr;
        std::unique_ptr<BindingGroup> bindingGroup = nullptr;
        std::unique_ptr<Buffer> mvpUniformBuffer = nullptr;
        std::unique_ptr<Buffer> instancingUniformBuffer = nullptr;
        std::unique_ptr<PipelineLayout> renderPipelineLayout = nullptr;
        std::unique_ptr<RenderPipeline> renderPipeline = nullptr;
        struct UBO
        {
            Instancing instacing;
        } ubo;
    } m_nonInstancing;

    struct
    {
        std::unique_ptr<BindingGroupLayout> bindingGroupLayout = nullptr;
        std::unique_ptr<BindingGroup> bindingGroup = nullptr;
        std::unique_ptr<Buffer> mvpUniformBuffer = nullptr;
        std::unique_ptr<PipelineLayout> renderPipelineLayout = nullptr;
        std::unique_ptr<RenderPipeline> renderPipeline = nullptr;
    } m_instancing;

    MVP m_mvp;
    Cube m_vertices = Cube(10.0f);
    std::vector<uint16_t> m_indices{
        0, 1, 3, // front
        3, 1, 2,
        1, 5, 2, // bottom
        2, 5, 6,
        5, 4, 6, // back
        6, 4, 7,
        4, 0, 7, // top
        7, 0, 3,
        3, 2, 7, // right
        7, 2, 6,
        4, 5, 0, // left
        0, 5, 1
    };
    std::vector<Instancing> m_instancings{};

    std::unique_ptr<Camera> m_camera = nullptr;
    uint32_t m_sampleCount = 1;

    struct
    {
        bool useInstancing = true;
        int instancingCount = 100;
        int instancingCountMax = 10000;
    } m_imguiSettings;
};

InstancingSample::InstancingSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

InstancingSample::~InstancingSample()
{
    clearImGui();

    m_instancing.renderPipeline.reset();
    m_instancing.renderPipelineLayout.reset();
    m_instancing.mvpUniformBuffer.reset();
    m_instancing.bindingGroup.reset();
    m_instancing.bindingGroupLayout.reset();

    m_nonInstancing.renderPipeline.reset();
    m_nonInstancing.renderPipelineLayout.reset();
    m_nonInstancing.mvpUniformBuffer.reset();
    m_nonInstancing.instancingUniformBuffer.reset();
    m_nonInstancing.bindingGroup.reset();
    m_nonInstancing.bindingGroupLayout.reset();

    m_indexBuffer.reset();
    m_instancingBuffer.reset();
    m_vertexBuffer.reset();
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

    createCamera(); // need size and aspect ratio from swapchain.

    createVertexBuffer();
    createInstancingBuffer();
    createIndexBuffer();

    createInstancingUniformBuffer();
    createInstancingBindingGroupLayout();
    createInstancingBindingGroup();
    createInstancingRenderPipeline();

    createNonInstancingUniformBuffer();
    createNonInstancingBindingGroupLayout();
    createNonInstancingBindingGroup();
    createNonInstancingRenderPipeline();

    initImGui(m_device.get(), m_queue.get(), m_swapchain.get());

    m_initialized = true;
}

void InstancingSample::updateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    MVP mvp{};
    mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.view = m_camera->getViewMat();
    mvp.proj = m_camera->getProjectionMat();

    void* pointer = m_instancing.mvpUniformBuffer->map();
    // memcpy(pointer, &mvp, sizeof(MVP));
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
        CommandEncoderDescriptor commandDescriptor{};
        auto commadEncoder = m_commandBuffer->createCommandEncoder(commandDescriptor);

        if (m_imguiSettings.useInstancing)
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

            auto renderPassEncoder = commadEncoder->beginRenderPass(renderPassDescriptor);
            renderPassEncoder->setPipeline(m_instancing.renderPipeline.get());
            renderPassEncoder->setBindingGroup(0, m_instancing.bindingGroup.get());
            renderPassEncoder->setVertexBuffer(VERTEX_SLOT, m_vertexBuffer.get());
            renderPassEncoder->setVertexBuffer(INSTANCING_SLOT, m_instancingBuffer.get());
            renderPassEncoder->setIndexBuffer(m_indexBuffer.get(), IndexFormat::kUint16);
            renderPassEncoder->setScissor(0, 0, m_width, m_height);
            renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
            renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_indices.size()), static_cast<uint32_t>(m_imguiSettings.instancingCount), 0, 0, 0);
            renderPassEncoder->end();
        }
        else
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

            auto renderPassEncoder = commadEncoder->beginRenderPass(renderPassDescriptor);
            renderPassEncoder->setPipeline(m_nonInstancing.renderPipeline.get());
            renderPassEncoder->setVertexBuffer(0, m_vertexBuffer.get());
            renderPassEncoder->setIndexBuffer(m_indexBuffer.get(), IndexFormat::kUint16);
            renderPassEncoder->setScissor(0, 0, m_width, m_height);
            renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);

            for (auto i = 0; i < m_imguiSettings.instancingCount; ++i)
            {
                m_nonInstancing.ubo.instacing.shift = glm::vec3(static_cast<float>(-i), static_cast<float>(-i), 0.0f);
                auto* bufferRef = m_nonInstancing.instancingUniformBuffer.get();
                void* pointer = bufferRef->map();
                memcpy(pointer, &m_nonInstancing.ubo.instacing.shift, bufferRef->getSize());
                // do not unmap.

                renderPassEncoder->setBindingGroup(0, m_nonInstancing.bindingGroup.get());
                renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
            }

            renderPassEncoder->end();
        }

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
    // set windows position and size
    {
        auto scale = ImGui::GetIO().FontGlobalScale;
        ImGui::SetNextWindowPos(ImVec2(20, 20 + m_padding.top), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300 * scale, 100 * scale), ImGuiCond_FirstUseEver);
    }

    // set ui
    {
        ImGui::Begin("Settings");
        ImGui::Checkbox("Use Instancing", &m_imguiSettings.useInstancing);
        ImGui::SliderInt("Instancing Count", &m_imguiSettings.instancingCount, 1, m_imguiSettings.instancingCountMax);
        ImGui::End();
    }

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
    descriptor.size = sizeof(Cube);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_vertexBuffer->map();
    memcpy(pointer, &m_vertices, descriptor.size);
    m_vertexBuffer->unmap();
}

void InstancingSample::createInstancingBuffer()
{
    // create instancing data.
    {
        auto halfWidth = m_swapchain->getWidth() / 2;
        auto halfHeight = m_swapchain->getHeight() / 2;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> xShiftDist(-halfWidth, halfWidth);
        std::uniform_int_distribution<> yShiftDist(-halfHeight, halfHeight);

        for (int i = 0; i < m_imguiSettings.instancingCountMax; i++)
        {
            auto xShift = xShiftDist(gen);
            auto yShift = yShiftDist(gen);

            // spdlog::trace("xShift: {}, yShift: {}", xShift, yShift);

            Instancing instancing;
            instancing.shift = glm::vec3(static_cast<float>(xShift), static_cast<float>(yShift), 0.0f);
            m_instancings.push_back(instancing);
        }
    }

    BufferDescriptor descriptor{};
    descriptor.size = m_instancings.size() * sizeof(Instancing);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_instancingBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_instancingBuffer->map();
    memcpy(pointer, m_instancings.data(), descriptor.size);
    // m_vertexBuffer->unmap();
}

void InstancingSample::createIndexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_indices.size() * sizeof(uint16_t);
    descriptor.usage = BufferUsageFlagBits::kIndex;

    m_indexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_indexBuffer->map();
    memcpy(pointer, m_indices.data(), descriptor.size);
    m_indexBuffer->unmap();
}

void InstancingSample::createInstancingBindingGroupLayout()
{
    BufferBindingLayout mvpBufferBindingLayout{};
    mvpBufferBindingLayout.index = 0;
    mvpBufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;
    mvpBufferBindingLayout.type = BufferBindingType::kUniform;

    BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
    bindingGroupLayoutDescriptor.buffers = { mvpBufferBindingLayout };

    m_instancing.bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
}

void InstancingSample::createInstancingBindingGroup()
{
    BufferBinding mvpBufferBinding{};
    mvpBufferBinding.index = 0;
    mvpBufferBinding.buffer = m_instancing.mvpUniformBuffer.get();
    mvpBufferBinding.offset = 0;
    mvpBufferBinding.size = m_instancing.mvpUniformBuffer->getSize();

    BindingGroupDescriptor bindingGroupDescriptor{};
    bindingGroupDescriptor.buffers = { mvpBufferBinding };
    bindingGroupDescriptor.layout = m_instancing.bindingGroupLayout.get();

    m_instancing.bindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
}

void InstancingSample::createInstancingUniformBuffer()
{
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 80.0f));
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 reori = R * T;
    // m_mvp.model = reori;
    m_mvp.model = glm::mat4(1.0f);
    m_mvp.view = m_camera->getViewMat();
    m_mvp.proj = m_camera->getProjectionMat();

    {
        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = sizeof(MVP);
        bufferDescriptor.usage = BufferUsageFlagBits::kUniform;

        m_instancing.mvpUniformBuffer = m_device->createBuffer(bufferDescriptor);
        void* mappedPointer = m_instancing.mvpUniformBuffer->map();
        memcpy(mappedPointer, &m_mvp, sizeof(MVP));
        // m_uniformBuffer->unmap();
    }
}

void InstancingSample::createInstancingRenderPipeline()
{
    // render pipeline layout
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_instancing.bindingGroupLayout.get() };

        m_instancing.renderPipelineLayout = m_device->createPipelineLayout(descriptor);
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
        positionAttribute.location = 0;
        positionAttribute.slot = VERTEX_SLOT;

        VertexAttribute colorAttribute{};
        colorAttribute.format = VertexFormat::kSFLOATx3;
        colorAttribute.offset = offsetof(Vertex, color);
        colorAttribute.location = 1;
        colorAttribute.slot = VERTEX_SLOT;

        VertexInputLayout vertexInputLayout{};
        vertexInputLayout.mode = VertexMode::kVertex;
        vertexInputLayout.stride = sizeof(Vertex);
        vertexInputLayout.attributes = { positionAttribute, colorAttribute };

        VertexAttribute shiftAttribute{};
        shiftAttribute.format = VertexFormat::kSFLOATx3;
        shiftAttribute.offset = offsetof(Instancing, shift);
        shiftAttribute.location = 2;
        shiftAttribute.slot = INSTANCING_SLOT;

        VertexInputLayout instancingInputLayout{};
        instancingInputLayout.mode = VertexMode::kInstance;
        instancingInputLayout.stride = sizeof(Instancing);
        instancingInputLayout.attributes = { shiftAttribute };

        vertexStage.entryPoint = "main";
        vertexStage.shaderModule = vertexShaderModule.get();
        vertexStage.layouts = { vertexInputLayout, instancingInputLayout };
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
    descriptor.layout = m_instancing.renderPipelineLayout.get();

    m_instancing.renderPipeline = m_device->createRenderPipeline(descriptor);
}

void InstancingSample::createNonInstancingBindingGroupLayout()
{
    BufferBindingLayout mvpBufferBindingLayout{};
    mvpBufferBindingLayout.index = 0;
    mvpBufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;
    mvpBufferBindingLayout.type = BufferBindingType::kUniform;

    BufferBindingLayout instancingBufferBindingLayout{};
    instancingBufferBindingLayout.index = 1;
    instancingBufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;
    instancingBufferBindingLayout.type = BufferBindingType::kUniform;

    BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
    bindingGroupLayoutDescriptor.buffers = { mvpBufferBindingLayout, instancingBufferBindingLayout };

    m_nonInstancing.bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
}

void InstancingSample::createNonInstancingBindingGroup()
{
    BufferBinding mvpBufferBinding{};
    mvpBufferBinding.index = 0;
    mvpBufferBinding.buffer = m_nonInstancing.mvpUniformBuffer.get();
    mvpBufferBinding.offset = 0;
    mvpBufferBinding.size = m_nonInstancing.mvpUniformBuffer->getSize();

    BufferBinding instancingBufferBinding{};
    instancingBufferBinding.index = 1;
    instancingBufferBinding.buffer = m_nonInstancing.instancingUniformBuffer.get();
    instancingBufferBinding.offset = 0;
    instancingBufferBinding.size = m_nonInstancing.instancingUniformBuffer->getSize();

    BindingGroupDescriptor bindingGroupDescriptor{};
    bindingGroupDescriptor.buffers = { mvpBufferBinding, instancingBufferBinding };
    bindingGroupDescriptor.layout = m_nonInstancing.bindingGroupLayout.get();

    m_nonInstancing.bindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
}

void InstancingSample::createNonInstancingUniformBuffer()
{
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 80.0f));
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 reori = R * T;
    // m_mvp.model = reori;
    m_mvp.model = glm::mat4(1.0f);
    m_mvp.view = m_camera->getViewMat();
    m_mvp.proj = m_camera->getProjectionMat();

    {
        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = sizeof(MVP);
        bufferDescriptor.usage = BufferUsageFlagBits::kUniform;

        m_nonInstancing.mvpUniformBuffer = m_device->createBuffer(bufferDescriptor);
        void* mappedPointer = m_nonInstancing.mvpUniformBuffer->map();
        memcpy(mappedPointer, &m_mvp, sizeof(MVP));
        // m_uniformBuffer->unmap();
    }

    {
        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = sizeof(Instancing);
        bufferDescriptor.usage = BufferUsageFlagBits::kUniform;

        m_nonInstancing.instancingUniformBuffer = m_device->createBuffer(bufferDescriptor);
        void* mappedPointer = m_nonInstancing.instancingUniformBuffer->map();
        memcpy(mappedPointer, &m_nonInstancing.ubo.instacing, sizeof(Instancing));
        // m_uniformBuffer->unmap();
    }
}

void InstancingSample::createNonInstancingRenderPipeline()
{
    // render pipeline layout
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { m_nonInstancing.bindingGroupLayout.get() };

        m_nonInstancing.renderPipelineLayout = m_device->createPipelineLayout(descriptor);
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
        std::vector<char> vertexShaderSource = utils::readFile(m_appDir / "non_instancing.vert.spv", m_handle);
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
        positionAttribute.location = 0;
        positionAttribute.slot = 0;

        VertexAttribute colorAttribute{};
        colorAttribute.format = VertexFormat::kSFLOATx3;
        colorAttribute.offset = offsetof(Vertex, color);
        colorAttribute.location = 1;
        colorAttribute.slot = 0;

        VertexInputLayout vertexInputLayout{};
        vertexInputLayout.mode = VertexMode::kVertex;
        vertexInputLayout.stride = sizeof(Vertex);
        vertexInputLayout.attributes = { positionAttribute, colorAttribute };

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
        std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "non_instancing.frag.spv", m_handle);
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
    descriptor.layout = m_nonInstancing.renderPipelineLayout.get();

    m_nonInstancing.renderPipeline = m_device->createRenderPipeline(descriptor);
}

void InstancingSample::createCamera()
{
    // m_camera = std::make_unique<PerspectiveCamera>(glm::radians(45.0f),
    //                                                m_swapchain->getWidth() / static_cast<float>(m_swapchain->getHeight()),
    //                                                0.1f,
    //                                                1000.0f);
    auto halfWidth = m_swapchain->getWidth() / 2.0f;
    auto halfHeight = m_swapchain->getHeight() / 2.0f;
    m_camera = std::make_unique<OrthographicCamera>(-halfWidth, halfWidth,
                                                    -halfHeight, halfHeight,
                                                    -1000, 1000);
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
