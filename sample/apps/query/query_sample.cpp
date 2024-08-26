#include "query_sample.h"

namespace jipu
{

QuerySample::QuerySample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
    // do not call init() function. it will be called in window exec() function.
}

QuerySample::~QuerySample()
{
    m_querySet.reset();
    m_renderPipeline.reset();
    m_renderPipelineLayout.reset();
    m_bindingGroup.reset();
    m_bindingGroupLayout.reset();
    m_vertexBuffer.reset();
    m_indexBuffer.reset();
    m_uniformBuffer.reset();
    m_commandBuffer.reset();
}

void QuerySample::init()
{
    Sample::init();

    createHPCWatcher();

    createCommandBuffer();

    createCamera(); // need size and aspect ratio from swapchain.

    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffer();
    createBindingGroupLayout();
    createBindingGroup();
    createRenderPipeline();
    createQuerySet();
}

void QuerySample::createCamera()
{
    m_camera = std::make_unique<PerspectiveCamera>(45.0f,
                                                   m_swapchain->getWidth() / static_cast<float>(m_swapchain->getHeight()),
                                                   0.1f,
                                                   1000.0f);

    // auto halfWidth = m_swapchain->getWidth() / 2.0f;
    // auto halfHeight = m_swapchain->getHeight() / 2.0f;
    // m_camera = std::make_unique<OrthographicCamera>(-halfWidth, halfWidth,
    //                                                 -halfHeight, halfHeight,
    //                                                 -1000, 1000);

    m_camera->lookAt(glm::vec3(0.0f, 0.0f, 1000.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0));
}

void QuerySample::updateUniformBuffer()
{
    m_ubo.mvp.model = glm::mat4(1.0f);
    m_ubo.mvp.view = m_camera->getViewMat();
    m_ubo.mvp.proj = m_camera->getProjectionMat();

    void* pointer = m_uniformBuffer->map(); // do not unmap.
    memcpy(pointer, &m_ubo, m_uniformBuffer->getSize());
}

void QuerySample::update()
{
    Sample::update();

    updateUniformBuffer();

    updateImGui();
}

void QuerySample::draw()
{
    auto& renderView = m_swapchain->acquireNextTexture();
    {
        ColorAttachment attachment{
            .renderView = renderView
        };
        attachment.clearValue = { .float32 = { 0.0, 0.0, 0.0, 0.0 } };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;

        RenderPassTimestampWrites timestampWrites = { .querySet = m_querySet.get(),
                                                      .beginQueryIndex = 0,
                                                      .endQueryIndex = 1 };

        RenderPassEncoderDescriptor renderPassDescriptor{
            .colorAttachments = { attachment },
            .timestampWrites = timestampWrites,
            .sampleCount = m_sampleCount
        };

        CommandEncoderDescriptor commandDescriptor{};
        auto commandEncoder = m_commandBuffer->createCommandEncoder(commandDescriptor);

        auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(*m_renderPipeline);
        renderPassEncoder->setBindingGroup(0, *m_bindingGroup);
        renderPassEncoder->setVertexBuffer(0, *m_vertexBuffer);
        renderPassEncoder->setIndexBuffer(*m_indexBuffer, IndexFormat::kUint16);
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->drawIndexed(static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
        renderPassEncoder->end();

        drawImGui(commandEncoder.get(), renderView);

        commandEncoder->resolveQuerySet(m_querySet.get(),
                                        0,
                                        m_querySet->getCount(),
                                        m_queryBuffer.get(),
                                        0);

        m_queue->submit({ commandEncoder->finish() }, *m_swapchain);

        auto pointer = reinterpret_cast<uint64_t*>(m_queryBuffer->map());

        for (uint32_t i = 1; i < m_querySet->getCount(); ++i)
        {
            uint64_t elapsedTime = (pointer[i] - pointer[i - 1]); // nano seconds
            double elapsedMilliSeconds = elapsedTime / 1000.0 / 1000.0;
            spdlog::debug("elapsed time {}", elapsedMilliSeconds);
        }
    }
}

void QuerySample::updateImGui()
{
    recordImGui({ [&]() {
        profilingWindow();
    } });
}

void QuerySample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void QuerySample::createVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_vertices.size() * sizeof(Vertex);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_vertexBuffer->map();
    memcpy(pointer, m_vertices.data(), descriptor.size);
    m_vertexBuffer->unmap();
}

void QuerySample::createIndexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_indices.size() * sizeof(uint16_t);
    descriptor.usage = BufferUsageFlagBits::kIndex;

    m_indexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_indexBuffer->map();
    memcpy(pointer, m_indices.data(), descriptor.size);
    m_indexBuffer->unmap();
}

void QuerySample::createUniformBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = sizeof(UBO);
    descriptor.usage = BufferUsageFlagBits::kUniform;

    m_uniformBuffer = m_device->createBuffer(descriptor);

    [[maybe_unused]] auto pointer = m_uniformBuffer->map();
    // memcpy(pointer, &m_ubo, descriptor.size);
    // m_uniformBuffer->unmap();
}

void QuerySample::createBindingGroupLayout()
{
    BufferBindingLayout bufferLayout{};
    bufferLayout.index = 0;
    bufferLayout.stages = BindingStageFlagBits::kVertexStage;
    bufferLayout.type = BufferBindingType::kUniform;

    BindingGroupLayoutDescriptor descriptor{};
    descriptor.buffers = { bufferLayout };

    m_bindingGroupLayout = m_device->createBindingGroupLayout(descriptor);
}

void QuerySample::createBindingGroup()
{
    BufferBinding bufferBinding{
        .index = 0,
        .offset = 0,
        .size = m_uniformBuffer->getSize(),
        .buffer = *m_uniformBuffer,
    };

    BindingGroupDescriptor descriptor{
        .layout = *m_bindingGroupLayout,
        .buffers = { bufferBinding },
    };

    m_bindingGroup = m_device->createBindingGroup(descriptor);
}

void QuerySample::createRenderPipeline()
{
    // render pipeline layout
    {
        PipelineLayoutDescriptor descriptor{};
        descriptor.layouts = { *m_bindingGroupLayout };

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
        { *m_renderPipelineLayout },
        inputAssemblyStage,
        vertexStage,
        rasterizationStage,
        fragmentStage
    };

    m_renderPipeline = m_device->createRenderPipeline(descriptor);
}

void QuerySample::createQuerySet()
{
    QuerySetDescriptor querySetDescriptor{};
    querySetDescriptor.count = 2;
    querySetDescriptor.type = QueryType::kTimestamp;

    m_querySet = m_device->createQuerySet(querySetDescriptor);

    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = querySetDescriptor.count * sizeof(uint64_t);
    bufferDescriptor.usage = BufferUsageFlagBits::kQueryResolve | BufferUsageFlagBits::kMapRead;

    m_queryBuffer = m_device->createBuffer(bufferDescriptor);
}

} // namespace jipu