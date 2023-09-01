
#include "file.h"
#include "sample.h"

#include "vkt/gpu/buffer.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/shader_module.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"

#include <chrono>
#include <cstdint>
#include <ctime>
#include <glm/glm.hpp>
#include <random>
#include <spdlog/spdlog.h>
#include <stdexcept>

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
    void draw() override;

private:
    void createDriver();
    void createPhysicalDevice();
    void createDevice();
    void createSurface();
    void createSwapchain();
    void createUniformBuffer();
    void createShaderStorageBuffer();
    void createColorAttachmentTexture();
    void createColorAttachmentTextureView();
    void createComputeBindingGroup();
    void createComputePipeline();
    void createRenderPipeline();
    void createQueue();
    void createCommandBuffer();

    void updateUniformBuffer();

    std::unique_ptr<CommandEncoder> recodeComputeCommandBuffer();
    std::unique_ptr<CommandEncoder> recodeRenderCommandBuffer();

private:
    struct Particle
    {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec4 color;
    };

    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;

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
    std::unique_ptr<Queue> m_queue = nullptr;

    std::vector<Particle> m_vertices{};

    void* m_uniformBufferMappedPointer = nullptr;
    uint32_t m_sampleCount = 1;
    uint32_t m_particleCount = 8092;
    uint64_t m_previousTime = 0;
    uint64_t m_vertexIndex = 0;
};

ParticleSample::ParticleSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

ParticleSample::~ParticleSample()
{
    m_renderCommandBuffer.reset();
    m_computeCommandBuffer.reset();
    m_queue.reset();

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

    m_swapchain.reset();
    m_surface.reset();

    m_device.reset();
    m_physicalDevice.reset();
    m_driver.reset();
}

void ParticleSample::init()
{
    createDriver();
    createPhysicalDevice();
    createDevice();

    createSurface();
    createSwapchain();

    createShaderStorageBuffer();
    createUniformBuffer();
    createColorAttachmentTexture();
    createColorAttachmentTextureView();

    createComputeBindingGroup();

    createComputePipeline();
    createRenderPipeline();

    createQueue();
    createCommandBuffer();
}

void ParticleSample::draw()
{
    updateUniformBuffer();

    std::unique_ptr<CommandEncoder> computeCommandEncoder = recodeComputeCommandBuffer();
    m_queue->submit({ computeCommandEncoder->finish() });

    std::unique_ptr<CommandEncoder> renderCommandEncoder = recodeRenderCommandBuffer();
    m_queue->submit({ renderCommandEncoder->finish() }, m_swapchain.get());

    m_vertexIndex = (m_vertexIndex + 1) % 2;
}

void ParticleSample::createDriver()
{
    DriverDescriptor descriptor{ .type = DriverType::VULKAN };
    m_driver = Driver::create(descriptor);
}

void ParticleSample::createPhysicalDevice()
{
    if (!m_driver)
        throw std::runtime_error("The driver instance is null pointer for physical device.");

    PhysicalDeviceDescriptor descriptor{ .index = 0 }; // TODO: use first physical device.
    m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
}

void ParticleSample::createDevice()
{
    if (!m_physicalDevice)
        throw std::runtime_error("The physical device instance is null pointer.");

    DeviceDescriptor descriptor{};
    m_device = m_physicalDevice->createDevice(descriptor);
}

void ParticleSample::createSurface()
{
    if (!m_driver)
        throw std::runtime_error("The driver instance is null pointer for surface.");

    SurfaceDescriptor descriptor{ .windowHandle = getWindowHandle() };
    m_surface = m_driver->createSurface(descriptor);
}

void ParticleSample::createSwapchain()
{
    if (!m_device)
        throw std::runtime_error("The device instance is null pointer for swapchain.");

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
    descriptor.usages = TextureUsageFlagBits::kColorAttachment;
    descriptor.width = m_swapchain->getWidth();
    descriptor.height = m_swapchain->getHeight();
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
        BufferBinding bufferUBOBinding{};
        bufferUBOBinding.buffer = m_uniformBuffer.get();
        bufferUBOBinding.index = 0;
        bufferUBOBinding.offset = 0;
        bufferUBOBinding.size = m_uniformBuffer->getSize();

        BufferBinding bufferInBinding{};
        bufferInBinding.buffer = m_vertexBuffers[0].get();
        bufferInBinding.index = 1;
        bufferInBinding.offset = 0;
        bufferInBinding.size = m_vertexBuffers[0]->getSize();

        BufferBinding bufferOutBinding{};
        bufferOutBinding.buffer = m_vertexBuffers[1].get();
        bufferOutBinding.index = 2;
        bufferOutBinding.offset = 0;
        bufferOutBinding.size = m_vertexBuffers[1]->getSize();

        BindingGroupDescriptor bindingGroupDescriptor{};
        bindingGroupDescriptor.layout = m_computeBindingGroupLayout.get();
        bindingGroupDescriptor.samplers = {};
        bindingGroupDescriptor.textures = {};
        bindingGroupDescriptor.buffers = {
            bufferUBOBinding,
            bufferInBinding,
            bufferOutBinding
        };

        auto computeBindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
        m_computeBindingGroups.push_back(std::move(computeBindingGroup));
    }

    {
        BufferBinding bufferUBOBinding{};
        bufferUBOBinding.buffer = m_uniformBuffer.get();
        bufferUBOBinding.index = 0;
        bufferUBOBinding.offset = 0;
        bufferUBOBinding.size = m_uniformBuffer->getSize();

        BufferBinding bufferInBinding{};
        bufferInBinding.buffer = m_vertexBuffers[1].get();
        bufferInBinding.index = 1;
        bufferInBinding.offset = 0;
        bufferInBinding.size = m_vertexBuffers[1]->getSize();

        BufferBinding bufferOutBinding{};
        bufferOutBinding.buffer = m_vertexBuffers[0].get();
        bufferOutBinding.index = 2;
        bufferOutBinding.offset = 0;
        bufferOutBinding.size = m_vertexBuffers[0]->getSize();

        BindingGroupDescriptor bindingGroupDescriptor{};
        bindingGroupDescriptor.layout = m_computeBindingGroupLayout.get();
        bindingGroupDescriptor.samplers = {};
        bindingGroupDescriptor.textures = {};
        bindingGroupDescriptor.buffers = {
            bufferUBOBinding,
            bufferOutBinding,
            bufferInBinding,
        };

        auto computeBindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
        m_computeBindingGroups.push_back(std::move(computeBindingGroup));
    }
}

void ParticleSample::createComputePipeline()
{
    // pipeline layout
    PipelineLayoutDescriptor pipelineLayoutDescriptor{};
    pipelineLayoutDescriptor.layouts = { m_computeBindingGroupLayout.get() };
    m_computePipelineLayout = m_device->createPipelineLayout(pipelineLayoutDescriptor);

    // compute shader
    ComputeStage computeStage{};
    const std::vector<char> computeShaderSource = utils::readFile(m_appDir / "particle.comp.spv", m_handle);
    ShaderModuleDescriptor shaderModuleDescriptor{ .code = computeShaderSource.data(),
                                                   .codeSize = computeShaderSource.size() };
    auto computeShader = m_device->createShaderModule(shaderModuleDescriptor);
    computeStage.entryPoint = "main";
    computeStage.shaderModule = computeShader.get();

    ComputePipelineDescriptor computePipelineDescriptor{};
    computePipelineDescriptor.compute = computeStage;
    computePipelineDescriptor.layout = m_computePipelineLayout.get();

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
    VertexStage vertexStage{};
    {
        // create vertex shader module.
        {
            const std::vector<char> vertexShaderSource = utils::readFile(m_appDir / "particle.vert.spv", m_handle);
            ShaderModuleDescriptor shaderModuleDescriptor{ .code = vertexShaderSource.data(),
                                                           .codeSize = vertexShaderSource.size() };
            m_vertexShaderModule = m_device->createShaderModule(shaderModuleDescriptor);

            vertexStage.entryPoint = "main";
            vertexStage.shaderModule = m_vertexShaderModule.get();
        }

        // create vertex shader layouts
        {
            std::vector<VertexAttribute> vertexAttributes{};
            vertexAttributes.resize(2);
            {
                // position
                vertexAttributes[0] = { .format = VertexFormat::kSFLOATx3,
                                        .offset = offsetof(Particle, position) };

                // texture coodinate
                vertexAttributes[1] = { .format = VertexFormat::kSFLOATx4,
                                        .offset = offsetof(Particle, color) };
            }

            VertexInputLayout vertexInputLayout{};
            vertexInputLayout.attributes = vertexAttributes;
            vertexInputLayout.mode = VertexMode::kVertex;
            vertexInputLayout.stride = sizeof(Particle);

            vertexStage.layouts = { vertexInputLayout };
        }
    }

    // rasterization
    RasterizationStage rasterizationStage{};
    {
        rasterizationStage.sampleCount = m_sampleCount;
    }

    // fragment shader
    FragmentStage fragmentStage{};
    {
        const std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "particle.frag.spv", m_handle);
        ShaderModuleDescriptor shaderModuleDescriptor{ .code = fragmentShaderSource.data(), .codeSize = fragmentShaderSource.size() };
        m_fragmentShaderModule = m_device->createShaderModule(shaderModuleDescriptor);

        fragmentStage.entryPoint = "main";
        fragmentStage.shaderModule = m_fragmentShaderModule.get();

        FragmentStage::Target target{};
        target.format = m_swapchain->getTextureFormat();

        fragmentStage.targets = { target };
    }

    RenderPipelineDescriptor renderPipelineDescriptor{};
    renderPipelineDescriptor.inputAssembly = inputAssembly;
    renderPipelineDescriptor.vertex = vertexStage;
    renderPipelineDescriptor.rasterization = rasterizationStage;
    renderPipelineDescriptor.fragment = fragmentStage;
    renderPipelineDescriptor.layout = m_renderPipelineLayout.get();
    m_renderPipeline = m_device->createRenderPipeline(renderPipelineDescriptor);
}

void ParticleSample::createCommandBuffer()
{
    CommandBufferDescriptor commandBufferDescriptor{ .usage = CommandBufferUsage::kOneTime };
    m_renderCommandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);
    m_computeCommandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);
}

void ParticleSample::createQueue()
{
    QueueDescriptor rednerQueueDescriptor{ .flags = QueueFlagBits::kGraphics | QueueFlagBits::kTransfer | QueueFlagBits::kCompute };
    m_queue = m_device->createQueue(rednerQueueDescriptor);
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

std::unique_ptr<CommandEncoder> ParticleSample::recodeComputeCommandBuffer()
{
    CommandEncoderDescriptor commandEncoderDescriptor{};
    std::unique_ptr<CommandEncoder> computeCommandEncoder = m_computeCommandBuffer->createCommandEncoder(commandEncoderDescriptor);

    ComputePassEncoderDescriptor computePassEncoderDescriptor{};
    std::unique_ptr<ComputePassEncoder> computePassEncoder = computeCommandEncoder->beginComputePass(computePassEncoderDescriptor);
    computePassEncoder->setPipeline(m_computePipeline.get());
    computePassEncoder->setBindingGroup(0, m_computeBindingGroups[(m_vertexIndex + 1) % 2].get());
    computePassEncoder->dispatch(256, 1, 1);
    computePassEncoder->end();

    return computeCommandEncoder;
}

std::unique_ptr<CommandEncoder> ParticleSample::recodeRenderCommandBuffer()
{
    auto swapchainImageIndex = m_swapchain->acquireNextTexture();

    if (swapchainImageIndex < 0)
        spdlog::error("swap chain: {}", swapchainImageIndex);

    CommandEncoderDescriptor commandEncoderDescriptor{};
    std::unique_ptr<CommandEncoder> renderCommandEncoder = m_renderCommandBuffer->createCommandEncoder(commandEncoderDescriptor);

    ColorAttachment colorAttachment{};
    colorAttachment.renderView = m_swapchain->getTextureView(swapchainImageIndex);
    colorAttachment.clearValue = { .float32 = { 0.0f, 0.0f, 0.0f, 1.0f } };
    colorAttachment.loadOp = LoadOp::kClear;
    colorAttachment.storeOp = StoreOp::kStore;

    RenderPassEncoderDescriptor renderPassEncoderDescriptor{};
    renderPassEncoderDescriptor.colorAttachments = { colorAttachment };

    std::unique_ptr<RenderPassEncoder> renderPassEncoder = renderCommandEncoder->beginRenderPass(renderPassEncoderDescriptor);
    renderPassEncoder->setPipeline(m_renderPipeline.get());
    renderPassEncoder->setVertexBuffer(m_vertexBuffers[m_vertexIndex].get());
    renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1); // set viewport state.
    renderPassEncoder->setScissor(0, 0, m_width, m_height);        // set scissor state.
    renderPassEncoder->draw(static_cast<uint32_t>(m_vertices.size()));
    renderPassEncoder->end();

    return renderCommandEncoder;
}
} // namespace vkt

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Particle Sample", app },
        ""
    };

    vkt::ParticleSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Particle Sample", nullptr },
        argv[0]
    };

    vkt::ParticleSample sample(descriptor);

    return sample.exec();
}

#endif
