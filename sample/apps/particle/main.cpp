
#include "file.h"
#include "sample.h"

#include "vkt/gpu/buffer.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/shader_module.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"

#include <glm/glm.hpp>
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
    void createRenderPipeline();
    void createVertexBuffer();
    void createCommandBuffer();

private:
    struct Vertex
    {
        glm::vec3 pos;
    };

    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;

    std::unique_ptr<ShaderModule> m_vertexShaderModule = nullptr;
    std::unique_ptr<ShaderModule> m_fragmentShaderModule = nullptr;
    std::unique_ptr<PipelineLayout> m_pipelineLayout = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<Buffer> m_indexBuffer = nullptr;

    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;

    uint32_t m_sampleCount = 1;
    std::vector<Vertex> m_vertices = {
        { { -0.5, 0.5, 0.0 } },  // left top
        { { -0.5, -0.5, 0.0 } }, // left bottom
        { { 0.5, -0.5, 0.0 } },  // right bottom
        { { 0.5, 0.5, 0.0 } }    // right top
    };

    std::vector<uint16_t> m_indices = { 0, 1, 2, 0, 2, 3 };
};

ParticleSample::ParticleSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

ParticleSample::~ParticleSample()
{
    m_commandBuffer.reset();

    m_indexBuffer.reset();
    m_vertexBuffer.reset();

    m_renderPipeline.reset();
    m_pipelineLayout.reset();

    m_fragmentShaderModule.reset();
    m_vertexShaderModule.reset();

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

    createRenderPipeline();

    createVertexBuffer();

    createCommandBuffer();
}

void ParticleSample::draw()
{
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

void ParticleSample::createRenderPipeline()
{
    // pipeline layout
    PipelineLayoutDescriptor pipelineLayoutdescriptor{};
    pipelineLayoutdescriptor.layouts = {};
    m_pipelineLayout = m_device->createPipelineLayout(pipelineLayoutdescriptor);

    // input assembly
    InputAssemblyStage inputAssembly{};
    inputAssembly.topology = PrimitiveTopology::kTriangleList;

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
            VertexAttribute vertexPositionAttribute{};
            vertexPositionAttribute.format = VertexFormat::kSFLOATx3;
            vertexPositionAttribute.offset = 0;

            VertexInputLayout vertexInputLayout{};
            vertexInputLayout.attributes = { vertexPositionAttribute };
            vertexInputLayout.mode = VertexMode::kVertex;
            vertexInputLayout.stride = sizeof(Vertex);
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
    renderPipelineDescriptor.layout = m_pipelineLayout.get();
    m_renderPipeline = m_device->createRenderPipeline(renderPipelineDescriptor);
}

void ParticleSample::createVertexBuffer()
{
    // create vertex buffer
    {
        uint64_t vertexSize = static_cast<uint64_t>(sizeof(Vertex) * m_vertices.size());
        BufferDescriptor vertexBufferDescriptor{};
        vertexBufferDescriptor.size = vertexSize;
        vertexBufferDescriptor.usage = BufferUsageFlagBits::kVertex;
        m_vertexBuffer = m_device->createBuffer(vertexBufferDescriptor);

        // TODO: currently buffer can be accessed both CPU and GPU.
        void* mappedPointer = m_vertexBuffer->map();
        memcpy(mappedPointer, m_vertices.data(), vertexSize);
        m_vertexBuffer->unmap();
    }

    // create index buffer
    {
        uint64_t indexSize = static_cast<uint64_t>(sizeof(uint16_t) * m_indices.size());
        BufferDescriptor indexBufferDescriptor{};
        indexBufferDescriptor.size = indexSize;
        indexBufferDescriptor.usage = BufferUsageFlagBits::kIndex;
        m_indexBuffer = m_device->createBuffer(indexBufferDescriptor);

        // TODO: currently buffer can be accessed both CPU and GPU.
        void* mappedPointer = m_indexBuffer->map();
        memcpy(mappedPointer, m_indices.data(), indexSize);
        m_indexBuffer->unmap();
    }
}

void ParticleSample::createCommandBuffer()
{
    CommandBufferDescriptor commandBufferDescriptor{ .usage = CommandBufferUsage::kOneTime };
    m_commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);
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
