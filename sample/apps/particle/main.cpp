
#include "file.h"
#include "sample.h"

#include "jipu/buffer.h"
#include "jipu/device.h"
#include "jipu/instance.h"
#include "jipu/physical_device.h"
#include "jipu/shader_module.h"
#include "jipu/surface.h"
#include "jipu/swapchain.h"

#include <chrono>
#include <cstdint>
#include <ctime>
#include <glm/glm.hpp>
#include <random>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

namespace
{

uint64_t getCurrentTime()
{
    return duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

} // namespace

class ParticleSample : public Sample
{
public:
    ParticleSample() = delete;
    ParticleSample(const SampleDescriptor& descriptor);
    ~ParticleSample() override;

public:
    void init() override;
    void update() override;
    void draw() override;

private:
    void updateImGui();

private:
    void createUniformBuffer();
    void createShaderStorageBuffer();
    void createColorAttachmentTexture();
    void createColorAttachmentTextureView();
    void createComputeBindingGroup();
    void createComputePipeline();
    void createRenderPipeline();
    void createCommandBuffer();

    void updateUniformBuffer();

private:
    struct Particle
    {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec4 color;
    };

    std::unique_ptr<BindingGroupLayout> m_computeBindingGroupLayout = nullptr;
    std::vector<std::unique_ptr<BindingGroup>> m_computeBindingGroups{};
    std::unique_ptr<BindingGroupLayout> m_renderBindingGroupLayout = nullptr;
    std::unique_ptr<BindingGroup> m_renderBindingGroup = nullptr;

    std::unique_ptr<ShaderModule> m_vertexShaderModule = nullptr;
    std::unique_ptr<ShaderModule> m_fragmentShaderModule = nullptr;
    std::unique_ptr<PipelineLayout> m_computePipelineLayout = nullptr;
    std::unique_ptr<ComputePipeline> m_computePipeline = nullptr;
    std::unique_ptr<PipelineLayout> m_renderPipelineLayout = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    std::unique_ptr<Texture> m_colorAttachmentTexture = nullptr;
    std::unique_ptr<TextureView> m_colorAttachmentTextureView = nullptr;

    std::vector<std::unique_ptr<Buffer>> m_vertexBuffers{};
    std::unique_ptr<Buffer> m_uniformBuffer = nullptr;

    std::unique_ptr<CommandBuffer> m_renderCommandBuffer = nullptr;
    std::unique_ptr<CommandBuffer> m_computeCommandBuffer = nullptr;

    std::vector<Particle> m_vertices{};

    void* m_uniformBufferMappedPointer = nullptr;
    uint32_t m_sampleCount = 1;
    uint32_t m_particleCount = 8192;
    uint64_t m_previousTime = 0;
    uint64_t m_vertexIndex = 0;

private:
    bool separateCmdBuffer = false;
};

ParticleSample::ParticleSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

ParticleSample::~ParticleSample()
{
    // release command buffer after finishing queue.
    m_renderCommandBuffer.reset();
    m_computeCommandBuffer.reset();

    m_renderPipeline.reset();
    m_renderPipelineLayout.reset();

    m_computePipeline.reset();
    m_computePipelineLayout.reset();

    m_fragmentShaderModule.reset();
    m_vertexShaderModule.reset();

    m_computeBindingGroups.clear();
    m_computeBindingGroupLayout.reset();

    m_colorAttachmentTextureView.reset();
    m_colorAttachmentTexture.reset();
    m_uniformBuffer.reset();
    m_vertexBuffers.clear();
}

void ParticleSample::init()
{
    Sample::init();

    createHPCWatcher();

    createShaderStorageBuffer();
    createUniformBuffer();
    createColorAttachmentTexture();
    createColorAttachmentTextureView();

    createComputeBindingGroup();

    createComputePipeline();
    createRenderPipeline();

    createCommandBuffer();
}

void ParticleSample::update()
{
    Sample::update();

    updateUniformBuffer();

    updateImGui();
}

void ParticleSample::updateImGui()
{
    recordImGui({ [&]() {
        windowImGui("Settings", { [&]() {
                        ImGui::Checkbox("Separate Command Buffer", &separateCmdBuffer);
                    } });
        profilingWindow();
    } });
}

void ParticleSample::draw()
{
    // encoder compute command
    {
        CommandEncoderDescriptor commandEncoderDescriptor{};
        std::unique_ptr<CommandEncoder> computeCommandEncoder = m_computeCommandBuffer->createCommandEncoder(commandEncoderDescriptor);

        ComputePassEncoderDescriptor computePassDescriptor{};
        std::unique_ptr<ComputePassEncoder> computePassEncoder = computeCommandEncoder->beginComputePass(computePassDescriptor);
        computePassEncoder->setPipeline(*m_computePipeline);
        computePassEncoder->setBindingGroup(0, *m_computeBindingGroups[(m_vertexIndex + 1) % 2]);
        computePassEncoder->dispatch(m_particleCount / 256, 1, 1);
        computePassEncoder->end();

        computeCommandEncoder->finish();

        if (separateCmdBuffer)
            m_queue->submit({ *m_computeCommandBuffer });
    }

    // encode render command
    {
        auto& renderView = m_swapchain->acquireNextTexture();

        CommandEncoderDescriptor commandEncoderDescriptor{};
        std::unique_ptr<CommandEncoder> renderCommandEncoder = m_renderCommandBuffer->createCommandEncoder(commandEncoderDescriptor);

        ColorAttachment colorAttachment{
            .renderView = renderView
        };
        colorAttachment.clearValue = { 0.0, 0.0, 0.0, 0.0 };
        colorAttachment.loadOp = LoadOp::kClear;
        colorAttachment.storeOp = StoreOp::kStore;

        RenderPassEncoderDescriptor renderPassDescriptor{
            .colorAttachments = { colorAttachment },
            .sampleCount = m_sampleCount
        };

        std::unique_ptr<RenderPassEncoder> renderPassEncoder = renderCommandEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(*m_renderPipeline);
        renderPassEncoder->setVertexBuffer(0, *m_vertexBuffers[m_vertexIndex]);
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1); // set viewport state.
        renderPassEncoder->setScissor(0, 0, m_width, m_height);        // set scissor state.
        renderPassEncoder->draw(static_cast<uint32_t>(m_vertices.size()));
        renderPassEncoder->end();

        drawImGui(renderCommandEncoder.get(), renderView);

        renderCommandEncoder->finish();

        if (separateCmdBuffer)
            m_queue->submit({ *m_renderCommandBuffer }, *m_swapchain);
    }

    if (!separateCmdBuffer)
        m_queue->submit({ *m_computeCommandBuffer, *m_renderCommandBuffer }, *m_swapchain);

    m_vertexIndex = (m_vertexIndex + 1) % 2;
}

void ParticleSample::createShaderStorageBuffer()
{
    // Initialize particles
    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

    // Initial particle positions on a circle
    m_vertices.resize(m_particleCount);
    for (auto& particle : m_vertices)
    {
        float r = 0.25f * sqrt(rndDist(rndEngine));
        float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
        float x = r * cos(theta) * m_height / m_height;
        float y = r * sin(theta);
        particle.position = glm::vec2(x, y);
        particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
        particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
    }

    // create vertex buffer
    uint64_t vertexSize = static_cast<uint64_t>(sizeof(Particle) * m_vertices.size());
    BufferDescriptor vertexBufferDescriptor{};
    vertexBufferDescriptor.size = vertexSize;
    vertexBufferDescriptor.usage = BufferUsageFlagBits::kVertex | BufferUsageFlagBits::kStorage;
    {
        auto vertexBuffer = m_device->createBuffer(vertexBufferDescriptor);
        void* mappedPointer = vertexBuffer->map();
        memcpy(mappedPointer, m_vertices.data(), vertexSize);
        vertexBuffer->unmap();

        m_vertexBuffers.push_back(std::move(vertexBuffer));
    }

    {
        auto vertexBuffer = m_device->createBuffer(vertexBufferDescriptor);
        void* mappedPointer = vertexBuffer->map();
        memcpy(mappedPointer, m_vertices.data(), vertexSize);
        vertexBuffer->unmap();

        m_vertexBuffers.push_back(std::move(vertexBuffer));
    }
}

void ParticleSample::createUniformBuffer()
{
    const uint64_t uniforBufferSize = sizeof(float);

    BufferDescriptor descriptor{};
    descriptor.size = uniforBufferSize;
    descriptor.usage = BufferUsageFlagBits::kUniform;
    m_uniformBuffer = m_device->createBuffer(descriptor);

    m_uniformBufferMappedPointer = m_uniformBuffer->map();
}

void ParticleSample::createColorAttachmentTexture()
{
    TextureDescriptor descriptor{};
    descriptor.format = m_swapchain->getTextureFormat();
    descriptor.type = TextureType::k2D;
    descriptor.usage = TextureUsageFlagBits::kColorAttachment;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
    descriptor.depth = 1;
    descriptor.mipLevels = 1;
    descriptor.sampleCount = m_sampleCount;

    m_colorAttachmentTexture = m_device->createTexture(descriptor);
}

void ParticleSample::createColorAttachmentTextureView()
{
    TextureViewDescriptor descriptor{};
    descriptor.type = TextureViewType::k2D;
    descriptor.aspect = TextureAspectFlagBits::kColor;

    m_colorAttachmentTextureView = m_colorAttachmentTexture->createTextureView(descriptor);
}

void ParticleSample::createComputeBindingGroup()
{
    BufferBindingLayout bufferUBOBindingLayout{};
    bufferUBOBindingLayout.index = 0;
    bufferUBOBindingLayout.stages = BindingStageFlagBits::kComputeStage;
    bufferUBOBindingLayout.type = BufferBindingType::kUniform;

    BufferBindingLayout bufferInBindingLayout{};
    bufferInBindingLayout.index = 1;
    bufferInBindingLayout.stages = BindingStageFlagBits::kVertexStage | BindingStageFlagBits::kComputeStage;
    bufferInBindingLayout.type = BufferBindingType::kStorage;

    BufferBindingLayout bufferOutBindingLayout{};
    bufferOutBindingLayout.index = 2;
    bufferOutBindingLayout.stages = BindingStageFlagBits::kVertexStage | BindingStageFlagBits::kComputeStage;
    bufferOutBindingLayout.type = BufferBindingType::kStorage;

    BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
    bindingGroupLayoutDescriptor.buffers = {
        bufferUBOBindingLayout,
        bufferInBindingLayout,
        bufferOutBindingLayout
    };
    m_computeBindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);

    {
        BufferBinding bufferUBOBinding{
            .index = 0,
            .offset = 0,
            .size = m_uniformBuffer->getSize(),
            .buffer = *m_uniformBuffer,
        };

        BufferBinding bufferInBinding{
            .index = 1,
            .offset = 0,
            .size = m_vertexBuffers[0]->getSize(),
            .buffer = *m_vertexBuffers[0],
        };

        BufferBinding bufferOutBinding{
            .index = 2,
            .offset = 0,
            .size = m_vertexBuffers[1]->getSize(),
            .buffer = *m_vertexBuffers[1],
        };

        BindingGroupDescriptor bindingGroupDescriptor{
            .layout = *m_computeBindingGroupLayout,
            .buffers = {
                bufferUBOBinding,
                bufferInBinding,
                bufferOutBinding,
            },
            .samplers = {},
            .textures = {},
        };

        auto computeBindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
        m_computeBindingGroups.push_back(std::move(computeBindingGroup));
    }

    {
        BufferBinding bufferUBOBinding{
            .index = 0,
            .offset = 0,
            .size = m_uniformBuffer->getSize(),
            .buffer = *m_uniformBuffer,
        };

        BufferBinding bufferInBinding{
            .index = 1,
            .offset = 0,
            .size = m_vertexBuffers[1]->getSize(),
            .buffer = *m_vertexBuffers[1],
        };

        BufferBinding bufferOutBinding{
            .index = 2,
            .offset = 0,
            .size = m_vertexBuffers[0]->getSize(),
            .buffer = *m_vertexBuffers[0],
        };

        BindingGroupDescriptor bindingGroupDescriptor{
            .layout = *m_computeBindingGroupLayout,
            .buffers = {
                bufferUBOBinding,
                bufferInBinding,
                bufferOutBinding,
            },
            .samplers = {},
            .textures = {},
        };

        auto computeBindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
        m_computeBindingGroups.push_back(std::move(computeBindingGroup));
    }
}

void ParticleSample::createComputePipeline()
{
    // pipeline layout
    PipelineLayoutDescriptor pipelineLayoutDescriptor{};
    pipelineLayoutDescriptor.layouts = { *m_computeBindingGroupLayout };
    m_computePipelineLayout = m_device->createPipelineLayout(pipelineLayoutDescriptor);

    // compute shader
    const std::vector<char> computeShaderSource = utils::readFile(m_appDir / "particle.comp.spv", m_handle);
    ShaderModuleDescriptor shaderModuleDescriptor{ .code = computeShaderSource.data(),
                                                   .codeSize = computeShaderSource.size() };
    auto computeShader = m_device->createShaderModule(shaderModuleDescriptor);
    ComputeStage computeStage{
        { *computeShader, "main" }
    };

    ComputePipelineDescriptor computePipelineDescriptor{
        { *m_computePipelineLayout },
        computeStage
    };

    m_computePipeline = m_device->createComputePipeline(computePipelineDescriptor);
}

void ParticleSample::createRenderPipeline()
{
    // pipeline layout
    PipelineLayoutDescriptor pipelineLayoutdescriptor{};
    pipelineLayoutdescriptor.layouts = {};
    m_renderPipelineLayout = m_device->createPipelineLayout(pipelineLayoutdescriptor);

    // input assembly
    InputAssemblyStage inputAssembly{};
    inputAssembly.topology = PrimitiveTopology::kPointList;

    // vertex shader

    // create vertex shader module.
    {
        const std::vector<char> vertexShaderSource = utils::readFile(m_appDir / "particle.vert.spv", m_handle);
        ShaderModuleDescriptor shaderModuleDescriptor{ .code = vertexShaderSource.data(),
                                                       .codeSize = vertexShaderSource.size() };
        m_vertexShaderModule = m_device->createShaderModule(shaderModuleDescriptor);
    }

    // create vertex shader layouts

    std::vector<VertexAttribute> vertexAttributes{};
    vertexAttributes.resize(2);
    {
        // position
        vertexAttributes[0] = { .format = VertexFormat::kSFLOATx3,
                                .offset = offsetof(Particle, position),
                                .location = 0 };

        // texture coodinate
        vertexAttributes[1] = { .format = VertexFormat::kSFLOATx4,
                                .offset = offsetof(Particle, color),
                                .location = 1 };
    }

    VertexInputLayout vertexInputLayout{};
    vertexInputLayout.attributes = vertexAttributes;
    vertexInputLayout.mode = VertexMode::kVertex;
    vertexInputLayout.stride = sizeof(Particle);

    VertexStage vertexStage{
        { *m_vertexShaderModule, "main" },
        { vertexInputLayout }
    };

    // rasterization
    RasterizationStage rasterizationStage{};
    {
        rasterizationStage.sampleCount = m_sampleCount;
        rasterizationStage.cullMode = CullMode::kBack;
        rasterizationStage.frontFace = FrontFace::kCounterClockwise;
    }

    // fragment shader

    const std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "particle.frag.spv", m_handle);
    ShaderModuleDescriptor fragShaderModuleDescriptor{ .code = fragmentShaderSource.data(), .codeSize = fragmentShaderSource.size() };
    m_fragmentShaderModule = m_device->createShaderModule(fragShaderModuleDescriptor);

    FragmentStage::Target target{};
    target.format = m_swapchain->getTextureFormat();

    FragmentStage fragmentStage{
        { *m_fragmentShaderModule, "main" },
        { target }
    };

    RenderPipelineDescriptor renderPipelineDescriptor{
        { *m_renderPipelineLayout },
        inputAssembly,
        vertexStage,
        rasterizationStage,
        fragmentStage
    };

    m_renderPipeline = m_device->createRenderPipeline(renderPipelineDescriptor);
}

void ParticleSample::createCommandBuffer()
{
    CommandBufferDescriptor commandBufferDescriptor{};
    m_renderCommandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);
    m_computeCommandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);
}

void ParticleSample::updateUniformBuffer()
{
    if (m_previousTime == 0)
    {
        m_previousTime = getCurrentTime();
    }

    uint64_t currentTime = getCurrentTime();

    float deltaTime = (currentTime - m_previousTime) * 10.0f;
    memcpy(m_uniformBufferMappedPointer, &deltaTime, sizeof(deltaTime));

    m_previousTime = currentTime;
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
        { 1000, 2000, "Particle Sample", app },
        ""
    };

    jipu::ParticleSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    jipu::SampleDescriptor descriptor{
        { 800, 600, "Particle Sample", nullptr },
        argv[0]
    };

    jipu::ParticleSample sample(descriptor);

    return sample.exec();
}

#endif
