#include "blend_sample.h"

namespace jipu
{

BlendSample::BlendSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
    // do not call init() function. it will be called in window exec() function.
}

BlendSample::~BlendSample()
{
    m_renderPipeline.reset();
    m_renderPipelineLayout.reset();
    m_bindingGroup.reset();
    m_bindingGroupLayout.reset();
    m_vertexBuffer.reset();
    m_indexBuffer.reset();
    m_uniformBuffer.reset();
    m_commandBuffer.reset();
}

void BlendSample::init()
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
}

void BlendSample::createCamera()
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

    m_ubo.mvp.model = glm::mat4(1.0f);
    m_ubo.mvp.view = m_camera->getViewMat();
    m_ubo.mvp.proj = m_camera->getProjectionMat();
}

void BlendSample::updateUniformBuffer()
{
    m_ubo.mvp.model = glm::mat4(1.0f);
    m_ubo.mvp.view = m_camera->getViewMat();
    m_ubo.mvp.proj = m_camera->getProjectionMat();

    void* pointer = m_uniformBuffer->map(); // do not unmap.
    memcpy(pointer, &m_ubo, m_uniformBuffer->getSize());
}

void BlendSample::update()
{
    Sample::update();

    // updateUniformBuffer();

    updateImGui();
}

void BlendSample::draw()
{
    auto& renderView = m_swapchain->acquireNextTexture();
    {
        ColorAttachment attachment{
            .renderView = renderView
        };
        attachment.clearValue = { .float32 = { 0.0, 0.0, 1.0, 0.0 } };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;

        RenderPassEncoderDescriptor renderPassDescriptor{
            .colorAttachments = { attachment },
            .occlusionQuerySet = nullptr,
            .timestampWrites = {},
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

        m_queue->submit({ commandEncoder->finish() }, *m_swapchain);
    }
}

void BlendSample::updateImGui()
{
    static const std::vector<std::string> opItems = { "kAdd", "kSubtract", "kReversSubtract", "kMin", "kMax" };
    static const std::vector<std::string> factorItems = { "kZero", "kOne", "kSrcColor", "kSrcAlpha", "kSrcAlphaSarurated",
                                                          "kOneMinusSrcColor", "kOneMinusSrcAlpha", "kDstColor", "kDstAlpha", "kOneMinusDstColor",
                                                          "kOneMinusDstAlpha", "kConstantColor", "kOneMinusConstantColor",
                                                          "kSrc1Color", "kOneMinusSrc1Color", "kSrc1Alpha", "kOneMinusSrc1Alpha" };
    auto combobox = [&](const char* label, const std::vector<std::string>& items, uint32_t* selected) {
        if (ImGui::BeginCombo(label, items[*selected].c_str()))
        {
            for (uint32_t i = 0; i < items.size(); ++i)
            {
                bool isSelected = *selected == i;
                if (ImGui::Selectable(items[i].c_str(), isSelected))
                {
                    *selected = i;
                    createRenderPipeline();
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    };

    recordImGui({ [&]() {
        windowImGui("Color Blend",
                    { [&]() {
                        combobox("Op", opItems, &m_blendColorOp);
                        combobox("Src Factor", factorItems, &m_blendColorSrcFactor);
                        combobox("Dst Factor", factorItems, &m_blendColorDstFactor);
                    } });
        windowImGui("Alpha Blend",
                    { [&]() {
                        combobox("Op", opItems, &m_blendAlphaOp);
                        combobox("Src Factor", factorItems, &m_blendAlphaSrcFactor);
                        combobox("Dst Factor", factorItems, &m_blendAlphaDstFactor);
                    } });
        profilingWindow();
    } });
}

void BlendSample::createCommandBuffer()
{
    CommandBufferDescriptor descriptor{};
    m_commandBuffer = m_device->createCommandBuffer(descriptor);
}

void BlendSample::createVertexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_vertices.size() * sizeof(Vertex);
    descriptor.usage = BufferUsageFlagBits::kVertex;

    m_vertexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_vertexBuffer->map();
    memcpy(pointer, m_vertices.data(), descriptor.size);
    m_vertexBuffer->unmap();
}

void BlendSample::createIndexBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = m_indices.size() * sizeof(uint16_t);
    descriptor.usage = BufferUsageFlagBits::kIndex;

    m_indexBuffer = m_device->createBuffer(descriptor);

    void* pointer = m_indexBuffer->map();
    memcpy(pointer, m_indices.data(), descriptor.size);
    m_indexBuffer->unmap();
}

void BlendSample::createUniformBuffer()
{
    BufferDescriptor descriptor{};
    descriptor.size = sizeof(UBO);
    descriptor.usage = BufferUsageFlagBits::kUniform;

    m_uniformBuffer = m_device->createBuffer(descriptor);

    auto pointer = m_uniformBuffer->map();
    memcpy(pointer, &m_ubo, descriptor.size);
    m_uniformBuffer->unmap();
}

void BlendSample::createBindingGroupLayout()
{
    BufferBindingLayout bufferLayout{};
    bufferLayout.index = 0;
    bufferLayout.stages = BindingStageFlagBits::kVertexStage;
    bufferLayout.type = BufferBindingType::kUniform;

    BindingGroupLayoutDescriptor descriptor{};
    descriptor.buffers = { bufferLayout };

    m_bindingGroupLayout = m_device->createBindingGroupLayout(descriptor);
}

void BlendSample::createBindingGroup()
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

void BlendSample::createRenderPipeline()
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
    target.blend = BlendState{
        .color = BlendComponent{
            .srcFactor = static_cast<BlendFactor>(m_blendColorSrcFactor),
            .dstFactor = static_cast<BlendFactor>(m_blendColorDstFactor),
            .operation = static_cast<BlendOperation>(m_blendColorOp),
        },
        .alpha = BlendComponent{
            .srcFactor = static_cast<BlendFactor>(m_blendAlphaSrcFactor),
            .dstFactor = static_cast<BlendFactor>(m_blendAlphaDstFactor),
            .operation = static_cast<BlendOperation>(m_blendAlphaOp),
        }
    };

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

} // namespace jipu