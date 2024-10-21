

#include "camera.h"
#include "file.h"
#include "sample.h"

#include "jipu/buffer.h"
#include "jipu/command_buffer.h"
#include "jipu/command_encoder.h"
#include "jipu/device.h"
#include "jipu/instance.h"
#include "jipu/physical_device.h"
#include "jipu/pipeline.h"
#include "jipu/pipeline_layout.h"
#include "jipu/queue.h"
#include "jipu/surface.h"
#include "jipu/swapchain.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <spdlog/spdlog.h>

static const uint32_t VERTEX_SLOT = 0;
static const uint32_t INSTANCING_SLOT = 1;

namespace jipu
{

class InstancingSample : public Sample
{
public:
    InstancingSample() = delete;
    InstancingSample(const SampleDescriptor& descriptor);
    ~InstancingSample() override;

    void init() override;
    void update() override;
    void draw() override;

private:
    void updateImGui();

    void updateUniformBuffer();

private:
    void createVertexBuffer();
    void createIndexBuffer();

    void createInstancingUniformBuffer();
    void createInstancingTransformBuffer();
    void createInstancingBindingGroupLayout();
    void createInstancingBindingGroup();
    void createInstancingRenderPipeline();

    void createNonInstancingUniformBuffer();
    void createNonInstancingTransformBuffer();
    void createNonInstancingBindingGroupLayout();
    void createNonInstancingBindingGroup();
    void createNonInstancingRenderPipeline();

    void createCamera();
    void createTransforms();

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

    struct UBO
    {
        MVP mvp;
        glm::mat4 orientation;
    };

    struct Transform
    {
        alignas(16) glm::vec3 translation;
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
    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<Buffer> m_indexBuffer = nullptr;

    struct
    {
        std::unique_ptr<BindingGroupLayout> bindingGroupLayout = nullptr;
        std::unique_ptr<BindingGroup> bindingGroup = nullptr;
        std::unique_ptr<Buffer> uniformBuffer = nullptr;
        std::unique_ptr<Buffer> transformBuffer = nullptr;
        std::unique_ptr<PipelineLayout> renderPipelineLayout = nullptr;
        std::unique_ptr<RenderPipeline> renderPipeline = nullptr;
    } m_nonInstancing;

    struct
    {
        std::unique_ptr<BindingGroupLayout> bindingGroupLayout = nullptr;
        std::unique_ptr<BindingGroup> bindingGroup = nullptr;
        std::unique_ptr<Buffer> uniformBuffer = nullptr;
        std::unique_ptr<Buffer> transformBuffer = nullptr;
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
    std::vector<Transform> m_transforms{};

    std::unique_ptr<Camera> m_camera = nullptr;
    uint32_t m_sampleCount = 1; // use only 1, because there is not resolve texture.

    struct
    {
        bool useInstancing = true;
        int objectCount = 1000;
        int maxObjectCount = 50000;
    } m_imguiSettings;
};

InstancingSample::InstancingSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

InstancingSample::~InstancingSample()
{
    m_instancing.renderPipeline.reset();
    m_instancing.renderPipelineLayout.reset();
    m_instancing.uniformBuffer.reset();
    m_instancing.transformBuffer.reset();
    m_instancing.bindingGroup.reset();
    m_instancing.bindingGroupLayout.reset();

    m_nonInstancing.renderPipeline.reset();
    m_nonInstancing.renderPipelineLayout.reset();
    m_nonInstancing.transformBuffer.reset();
    m_nonInstancing.uniformBuffer.reset();
    m_nonInstancing.bindingGroup.reset();
    m_nonInstancing.bindingGroupLayout.reset();

    m_indexBuffer.reset();
    m_vertexBuffer.reset();
}

void InstancingSample::init()
{
    Sample::init();

    createHPCWatcher();

    createCamera(); // need size and aspect ratio from swapchain.
    createTransforms();

    createVertexBuffer();
    createIndexBuffer();

    createInstancingUniformBuffer();
    createInstancingTransformBuffer();
    createInstancingBindingGroupLayout();
    createInstancingBindingGroup();
    createInstancingRenderPipeline();

    createNonInstancingUniformBuffer();
    createNonInstancingTransformBuffer();
    createNonInstancingBindingGroupLayout();
    createNonInstancingBindingGroup();
    createNonInstancingRenderPipeline();
}

void InstancingSample::updateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UBO ubo{};
    ubo.mvp.model = glm::mat4(1.0f);
    ubo.mvp.view = m_camera->getViewMat();
    ubo.mvp.proj = m_camera->getProjectionMat();
    ubo.orientation = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    void* pointer = m_instancing.uniformBuffer->map();
    memcpy(pointer, &ubo, sizeof(UBO));
    pointer = m_nonInstancing.uniformBuffer->map();
    memcpy(pointer, &ubo, sizeof(UBO));
}

void InstancingSample::update()
{
    Sample::update();

    updateUniformBuffer();

    updateImGui();
}

void InstancingSample::draw()
{
    auto renderView = m_swapchain->acquireNextTextureView();
    {
        auto commandEncoder = m_device->createCommandEncoder(CommandEncoderDescriptor{});

        ColorAttachment attachment{
            .renderView = renderView
        };
        attachment.clearValue = { 0.0, 0.0, 0.0, 0.0 };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;

        RenderPassEncoderDescriptor renderPassDescriptor{
            .colorAttachments = { attachment }
        };

        if (m_imguiSettings.useInstancing)
        {
            auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassDescriptor);
            renderPassEncoder->setPipeline(m_instancing.renderPipeline.get());
            renderPassEncoder->setBindingGroup(0, *m_instancing.bindingGroup);
            renderPassEncoder->setVertexBuffer(VERTEX_SLOT, *m_vertexBuffer);
            renderPassEncoder->setVertexBuffer(INSTANCING_SLOT, *m_instancing.transformBuffer);
            renderPassEncoder->setIndexBuffer(*m_indexBuffer, IndexFormat::kUint16);
            renderPassEncoder->setScissor(0, 0, m_width, m_height);
            renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
            renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_indices.size()), static_cast<uint32_t>(m_imguiSettings.objectCount), 0, 0, 0);
            renderPassEncoder->end();

            drawImGui(commandEncoder.get(), *renderView);
            auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});

            m_queue->submit({ commandBuffer.get() });
            m_swapchain->present();
        }
        else
        {
            auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassDescriptor);
            renderPassEncoder->setPipeline(m_nonInstancing.renderPipeline.get());
            renderPassEncoder->setVertexBuffer(0, *m_vertexBuffer);
            renderPassEncoder->setIndexBuffer(*m_indexBuffer, IndexFormat::kUint16);
            renderPassEncoder->setScissor(0, 0, m_width, m_height);
            renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
            for (auto i = 0; i < m_imguiSettings.objectCount; ++i)
            {
                uint32_t offset = i * sizeof(Transform);
                renderPassEncoder->setBindingGroup(0, *m_nonInstancing.bindingGroup, { offset });
                renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
            }
            renderPassEncoder->end();

            drawImGui(commandEncoder.get(), *renderView);

            auto commandBuffer = commandEncoder->finish(CommandBufferDescriptor{});
            m_queue->submit({ commandBuffer.get() });
            m_swapchain->present();
        }
    }
}

void InstancingSample::updateImGui()
{
    recordImGui({ [&]() {
        windowImGui("Settings", { [&]() {
                        ImGui::Checkbox("Use Instancing", &m_imguiSettings.useInstancing);
                        ImGui::SliderInt("Number of Object", &m_imguiSettings.objectCount, 1, m_imguiSettings.maxObjectCount);
                    } });
        profilingWindow();
    } });
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
        bufferDescriptor.size = sizeof(UBO);
        bufferDescriptor.usage = BufferUsageFlagBits::kUniform;

        m_instancing.uniformBuffer = m_device->createBuffer(bufferDescriptor);
        void* mappedPointer = m_instancing.uniformBuffer->map();
        memcpy(mappedPointer, &m_mvp, sizeof(UBO));
        // m_uniformBuffer->unmap();
    }
}

void InstancingSample::createInstancingTransformBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_transforms.size() * sizeof(Transform);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_instancing.transformBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_instancing.transformBuffer->map();
    memcpy(pointer, m_transforms.data(), descriptor.size);
    m_vertexBuffer->unmap();
}

void InstancingSample::createInstancingBindingGroupLayout()
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

    m_instancing.bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
}

void InstancingSample::createInstancingBindingGroup()
{
    BufferBinding mvpBufferBinding{
        .index = 0,
        .offset = 0,
        .size = m_instancing.uniformBuffer->getSize(),
        .buffer = m_instancing.uniformBuffer.get(),
    };

    BindingGroupDescriptor bindingGroupDescriptor{
        .layout = m_instancing.bindingGroupLayout.get(),
        .buffers = { mvpBufferBinding }
    };

    m_instancing.bindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
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

    VertexAttribute positionAttribute{};
    positionAttribute.format = VertexFormat::kFloat32x3;
    positionAttribute.offset = offsetof(Vertex, pos);
    positionAttribute.location = 0;
    positionAttribute.slot = VERTEX_SLOT;

    VertexAttribute colorAttribute{};
    colorAttribute.format = VertexFormat::kFloat32x3;
    colorAttribute.offset = offsetof(Vertex, color);
    colorAttribute.location = 1;
    colorAttribute.slot = VERTEX_SLOT;

    VertexInputLayout vertexInputLayout{};
    vertexInputLayout.mode = VertexMode::kVertex;
    vertexInputLayout.stride = sizeof(Vertex);
    vertexInputLayout.attributes = { positionAttribute, colorAttribute };

    VertexAttribute shiftAttribute{};
    shiftAttribute.format = VertexFormat::kFloat32x3;
    shiftAttribute.offset = offsetof(Transform, translation);
    shiftAttribute.location = 2;
    shiftAttribute.slot = INSTANCING_SLOT;

    VertexInputLayout instancingInputLayout{};
    instancingInputLayout.mode = VertexMode::kInstance;
    instancingInputLayout.stride = sizeof(Transform);
    instancingInputLayout.attributes = { shiftAttribute };

    VertexStage vertexStage{
        { vertexShaderModule.get(), "main" },
        { vertexInputLayout, instancingInputLayout }
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
        std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "instancing.frag.spv", m_handle);
        descriptor.code = fragmentShaderSource.data();
        descriptor.codeSize = static_cast<uint32_t>(fragmentShaderSource.size());

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
        m_instancing.renderPipelineLayout.get(),
        inputAssemblyStage,
        vertexStage,
        rasterizationStage,
        fragmentStage
    };

    m_instancing.renderPipeline = m_device->createRenderPipeline(descriptor);
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

    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = sizeof(UBO);
    bufferDescriptor.usage = BufferUsageFlagBits::kUniform;

    m_nonInstancing.uniformBuffer = m_device->createBuffer(bufferDescriptor);
    void* mappedPointer = m_nonInstancing.uniformBuffer->map();
    memcpy(mappedPointer, &m_mvp, sizeof(UBO));
    m_nonInstancing.uniformBuffer->unmap();
}

void InstancingSample::createNonInstancingTransformBuffer()
{
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = sizeof(Transform) * m_transforms.size();
    bufferDescriptor.usage = BufferUsageFlagBits::kUniform;

    m_nonInstancing.transformBuffer = m_device->createBuffer(bufferDescriptor);
    void* mappedPointer = m_nonInstancing.transformBuffer->map();
    memcpy(mappedPointer, m_transforms.data(), bufferDescriptor.size);
    m_nonInstancing.transformBuffer->unmap();
}

void InstancingSample::createNonInstancingBindingGroupLayout()
{
    BufferBindingLayout mvpBufferBindingLayout{};
    mvpBufferBindingLayout.index = 0;
    mvpBufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;
    mvpBufferBindingLayout.type = BufferBindingType::kUniform;

    BufferBindingLayout instancingBufferBindingLayout{};
    instancingBufferBindingLayout.index = 1;
    instancingBufferBindingLayout.dynamicOffset = true;
    instancingBufferBindingLayout.stages = BindingStageFlagBits::kVertexStage;
    instancingBufferBindingLayout.type = BufferBindingType::kUniform;

    BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
    bindingGroupLayoutDescriptor.buffers = { mvpBufferBindingLayout, instancingBufferBindingLayout };

    m_nonInstancing.bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
}

void InstancingSample::createNonInstancingBindingGroup()
{
    BufferBinding bufferBinding{
        .index = 0,
        .offset = 0,
        .size = m_nonInstancing.uniformBuffer->getSize(),
        .buffer = m_nonInstancing.uniformBuffer.get(),
    };

    BufferBinding instancingBufferBinding{
        .index = 1,
        .offset = 0,
        .size = m_nonInstancing.transformBuffer->getSize() / m_transforms.size(),
        .buffer = m_nonInstancing.transformBuffer.get(),
    };

    BindingGroupDescriptor bindingGroupDescriptor{
        .layout = m_nonInstancing.bindingGroupLayout.get(),
        .buffers = { bufferBinding, instancingBufferBinding }
    };

    m_nonInstancing.bindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
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

    VertexAttribute positionAttribute{};
    positionAttribute.format = VertexFormat::kFloat32x3;
    positionAttribute.offset = offsetof(Vertex, pos);
    positionAttribute.location = 0;
    positionAttribute.slot = 0;

    VertexAttribute colorAttribute{};
    colorAttribute.format = VertexFormat::kFloat32x3;
    colorAttribute.offset = offsetof(Vertex, color);
    colorAttribute.location = 1;
    colorAttribute.slot = 0;

    VertexInputLayout vertexInputLayout{};
    vertexInputLayout.mode = VertexMode::kVertex;
    vertexInputLayout.stride = sizeof(Vertex);
    vertexInputLayout.attributes = { positionAttribute, colorAttribute };

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
        ShaderModuleDescriptor descriptor{};
        std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "non_instancing.frag.spv", m_handle);
        descriptor.code = fragmentShaderSource.data();
        descriptor.codeSize = static_cast<uint32_t>(fragmentShaderSource.size());

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
        m_nonInstancing.renderPipelineLayout.get(),
        inputAssemblyStage,
        vertexStage,
        rasterizationStage,
        fragmentStage
    };

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
                                                    -1000.0f, 1000.0f);
    m_camera->lookAt(glm::vec3(0.0f, 0.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

void InstancingSample::createTransforms()
{
    auto halfWidth = m_swapchain->getWidth() / 2;
    auto halfHeight = m_swapchain->getHeight() / 2;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xTranslationDist(-halfWidth, halfWidth);
    std::uniform_int_distribution<> yTranslationDist(-halfHeight, halfHeight);

    for (int i = 0; i < m_imguiSettings.maxObjectCount; i++)
    {
        auto xTranslation = xTranslationDist(gen);
        auto yTranslation = yTranslationDist(gen);

        Transform transform;
        transform.translation = glm::vec3(static_cast<float>(xTranslation), static_cast<float>(yTranslation), 0.0f);
        m_transforms.push_back(transform);
    }
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
        { 1000, 2000, "Instancing", app },
        ""
    };

    jipu::InstancingSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    jipu::SampleDescriptor descriptor{
        { 800, 600, "Instancing", nullptr },
        argv[0]
    };

    jipu::InstancingSample sample(descriptor);

    return sample.exec();
}

#endif
