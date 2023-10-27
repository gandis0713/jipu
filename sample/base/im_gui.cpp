#include "im_gui.h"

namespace vkt
{

/*
#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outUV = inUV;
    outColor = inColor;
    gl_Position = vec4(inPos * vec2(2.0f / 800.0f, 2.0f / 600.0) + vec2(-1.0, -1.0), 0.0, 1.0);
}
*/

static std::vector<uint32_t> vertexShaderSourceSpv = { 0x07230203, 0x00010000, 0x000d000b, 0x00000027, 0x00000000, 0x00020011, 0x00000001, 0x0006000b, 0x00000001, 0x4c534c47, 0x6474732e, 0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001, 0x000b000f, 0x00000000, 0x00000004, 0x6e69616d, 0x00000000, 0x00000009, 0x0000000b, 0x0000000f, 0x00000011, 0x00000015, 0x00000018, 0x00030003, 0x00000002, 0x000001c2, 0x000a0004, 0x475f4c47, 0x4c474f4f, 0x70635f45, 0x74735f70, 0x5f656c79, 0x656e696c, 0x7269645f, 0x69746365, 0x00006576, 0x00080004, 0x475f4c47, 0x4c474f4f, 0x6e695f45, 0x64756c63, 0x69645f65, 0x74636572, 0x00657669, 0x00040005, 0x00000004, 0x6e69616d, 0x00000000, 0x00040005, 0x00000009, 0x5574756f, 0x00000056, 0x00040005, 0x0000000b, 0x56556e69, 0x00000000, 0x00050005, 0x0000000f, 0x4374756f, 0x726f6c6f, 0x00000000, 0x00040005, 0x00000011, 0x6f436e69, 0x00726f6c, 0x00060005, 0x00000013, 0x505f6c67, 0x65567265, 0x78657472, 0x00000000, 0x00060006, 0x00000013, 0x00000000, 0x505f6c67, 0x7469736f, 0x006e6f69, 0x00030005, 0x00000015, 0x00000000, 0x00040005, 0x00000018, 0x6f506e69, 0x00000073, 0x00040047, 0x00000009, 0x0000001e, 0x00000000, 0x00040047, 0x0000000b, 0x0000001e, 0x00000001, 0x00040047, 0x0000000f, 0x0000001e, 0x00000001, 0x00040047, 0x00000011, 0x0000001e, 0x00000002, 0x00050048, 0x00000013, 0x00000000, 0x0000000b, 0x00000000, 0x00030047, 0x00000013, 0x00000002, 0x00040047, 0x00000018, 0x0000001e, 0x00000000, 0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002, 0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006, 0x00000002, 0x00040020, 0x00000008, 0x00000003, 0x00000007, 0x0004003b, 0x00000008, 0x00000009, 0x00000003, 0x00040020, 0x0000000a, 0x00000001, 0x00000007, 0x0004003b, 0x0000000a, 0x0000000b, 0x00000001, 0x00040017, 0x0000000d, 0x00000006, 0x00000004, 0x00040020, 0x0000000e, 0x00000003, 0x0000000d, 0x0004003b, 0x0000000e, 0x0000000f, 0x00000003, 0x00040020, 0x00000010, 0x00000001, 0x0000000d, 0x0004003b, 0x00000010, 0x00000011, 0x00000001, 0x0003001e, 0x00000013, 0x0000000d, 0x00040020, 0x00000014, 0x00000003, 0x00000013, 0x0004003b, 0x00000014, 0x00000015, 0x00000003, 0x00040015, 0x00000016, 0x00000020, 0x00000001, 0x0004002b, 0x00000016, 0x00000017, 0x00000000, 0x0004003b, 0x0000000a, 0x00000018, 0x00000001, 0x0004002b, 0x00000006, 0x0000001a, 0x3b23d70a, 0x0004002b, 0x00000006, 0x0000001b, 0x3b5a740e, 0x0005002c, 0x00000007, 0x0000001c, 0x0000001a, 0x0000001b, 0x0004002b, 0x00000006, 0x0000001e, 0xbf800000, 0x0005002c, 0x00000007, 0x0000001f, 0x0000001e, 0x0000001e, 0x0004002b, 0x00000006, 0x00000021, 0x00000000, 0x0004002b, 0x00000006, 0x00000022, 0x3f800000, 0x00050036, 0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x0004003d, 0x00000007, 0x0000000c, 0x0000000b, 0x0003003e, 0x00000009, 0x0000000c, 0x0004003d, 0x0000000d, 0x00000012, 0x00000011, 0x0003003e, 0x0000000f, 0x00000012, 0x0004003d, 0x00000007, 0x00000019, 0x00000018, 0x00050085, 0x00000007, 0x0000001d, 0x00000019, 0x0000001c, 0x00050081, 0x00000007, 0x00000020, 0x0000001d, 0x0000001f, 0x00050051, 0x00000006, 0x00000023, 0x00000020, 0x00000000, 0x00050051, 0x00000006, 0x00000024, 0x00000020, 0x00000001, 0x00070050, 0x0000000d, 0x00000025, 0x00000023, 0x00000024, 0x00000021, 0x00000022, 0x00050041, 0x0000000e, 0x00000026, 0x00000015, 0x00000017, 0x0003003e, 0x00000026, 0x00000025, 0x000100fd, 0x00010038 };

/*
#version 450

layout(binding = 0) uniform sampler2D fontSampler;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = inColor * texture(fontSampler, inUV);
}
*/
static std::vector<uint32_t> fragmentShaderSourceSpv = { 0x07230203, 0x00010000, 0x000d000b, 0x00000018, 0x00000000, 0x00020011, 0x00000001, 0x0006000b, 0x00000001, 0x4c534c47, 0x6474732e, 0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001, 0x0008000f, 0x00000004, 0x00000004, 0x6e69616d, 0x00000000, 0x00000009, 0x0000000b, 0x00000014, 0x00030010, 0x00000004, 0x00000007, 0x00030003, 0x00000002, 0x000001c2, 0x000a0004, 0x475f4c47, 0x4c474f4f, 0x70635f45, 0x74735f70, 0x5f656c79, 0x656e696c, 0x7269645f, 0x69746365, 0x00006576, 0x00080004, 0x475f4c47, 0x4c474f4f, 0x6e695f45, 0x64756c63, 0x69645f65, 0x74636572, 0x00657669, 0x00040005, 0x00000004, 0x6e69616d, 0x00000000, 0x00050005, 0x00000009, 0x4374756f, 0x726f6c6f, 0x00000000, 0x00040005, 0x0000000b, 0x6f436e69, 0x00726f6c, 0x00050005, 0x00000010, 0x746e6f66, 0x706d6153, 0x0072656c, 0x00040005, 0x00000014, 0x56556e69, 0x00000000, 0x00040047, 0x00000009, 0x0000001e, 0x00000000, 0x00040047, 0x0000000b, 0x0000001e, 0x00000001, 0x00040047, 0x00000010, 0x00000022, 0x00000000, 0x00040047, 0x00000010, 0x00000021, 0x00000000, 0x00040047, 0x00000014, 0x0000001e, 0x00000000, 0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002, 0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006, 0x00000004, 0x00040020, 0x00000008, 0x00000003, 0x00000007, 0x0004003b, 0x00000008, 0x00000009, 0x00000003, 0x00040020, 0x0000000a, 0x00000001, 0x00000007, 0x0004003b, 0x0000000a, 0x0000000b, 0x00000001, 0x00090019, 0x0000000d, 0x00000006, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 0x0003001b, 0x0000000e, 0x0000000d, 0x00040020, 0x0000000f, 0x00000000, 0x0000000e, 0x0004003b, 0x0000000f, 0x00000010, 0x00000000, 0x00040017, 0x00000012, 0x00000006, 0x00000002, 0x00040020, 0x00000013, 0x00000001, 0x00000012, 0x0004003b, 0x00000013, 0x00000014, 0x00000001, 0x00050036, 0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x0004003d, 0x00000007, 0x0000000c, 0x0000000b, 0x0004003d, 0x0000000e, 0x00000011, 0x00000010, 0x0004003d, 0x00000012, 0x00000015, 0x00000014, 0x00050057, 0x00000007, 0x00000016, 0x00000011, 0x00000015, 0x00050085, 0x00000007, 0x00000017, 0x0000000c, 0x00000016, 0x0003003e, 0x00000009, 0x00000017, 0x000100fd, 0x00010038 };

void Im_Gui::initImGui(Device* device, Queue* queue, Swapchain* swapchain)
{
    m_device = device;
    m_queue = queue;
    m_swapchain = swapchain;

    // IMGUI_CHECKVERSION();
    ImGuiContext* imguiContext = ImGui::CreateContext();
    if (imguiContext == nullptr)
    {
        throw std::runtime_error("Failed to create imgui context");
    }

    auto scale = 1.0f;
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = scale;
    io.DisplaySize = ImVec2(swapchain->getWidth(), swapchain->getHeight());
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

    // Get texture for fonts.
    using FontDataType = unsigned char;
    FontDataType* fontData = nullptr;
    int fontTexWidth, fontTexHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &fontTexWidth, &fontTexHeight);

    // create font texture.
    {
        TextureDescriptor fontTextureDescriptor{};
        fontTextureDescriptor.type = TextureType::k2D;
        fontTextureDescriptor.format = TextureFormat::kRGBA_8888_UInt_Norm;
        fontTextureDescriptor.width = fontTexWidth;
        fontTextureDescriptor.height = fontTexHeight;
        fontTextureDescriptor.mipLevels = 1;
        fontTextureDescriptor.sampleCount = 1;
        fontTextureDescriptor.usage = TextureUsageFlagBits::kCopyDst |
                                      TextureUsageFlagBits::kTextureBinding;

        m_fontTexture = device->createTexture(fontTextureDescriptor);
    }

    // create font texture view.
    {
        TextureViewDescriptor fontTextureViewDescriptor{};
        fontTextureViewDescriptor.aspect = TextureAspectFlagBits::kColor;
        fontTextureViewDescriptor.type = TextureViewType::k2D;

        m_fontTextureView = m_fontTexture->createTextureView(fontTextureViewDescriptor);
    }

    // create font staging buffer.
    std::unique_ptr<Buffer> m_fontBuffer = nullptr;
    {
        BufferDescriptor fontBufferDescriptor{};
        fontBufferDescriptor.size = fontTexWidth * fontTexHeight * 4 * sizeof(FontDataType);
        fontBufferDescriptor.usage = BufferUsageFlagBits::kCopySrc;

        m_fontBuffer = device->createBuffer(fontBufferDescriptor);

        void* fontGPUMappedPointer = m_fontBuffer->map();
        memcpy(fontGPUMappedPointer, fontData, fontBufferDescriptor.size);
        // m_fontBuffer->unmap();
    }

    // copy buffer to texture
    {
        BlitTextureBuffer blitTextureBuffer{};
        blitTextureBuffer.buffer = m_fontBuffer.get();
        blitTextureBuffer.offset = 0;
        uint32_t channel = 4;
        uint32_t bytesPerData = sizeof(FontDataType);
        blitTextureBuffer.bytesPerRow = bytesPerData * m_fontTexture->getWidth() * channel;
        blitTextureBuffer.rowsPerTexture = m_fontTexture->getHeight();

        BlitTexture blitTexture{ .texture = m_fontTexture.get() };
        Extent3D extent{};
        extent.width = m_fontTexture->getWidth();
        extent.height = m_fontTexture->getHeight();
        extent.depth = 1;

        CommandBufferDescriptor commandBufferDescriptor{ .usage = CommandBufferUsage::kOneTime };
        std::unique_ptr<CommandBuffer> commandBuffer = device->createCommandBuffer(commandBufferDescriptor);

        CommandEncoderDescriptor commandEncoderDescriptor{};
        std::unique_ptr<CommandEncoder> commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);
        commandEncoder->copyBufferToTexture(blitTextureBuffer, blitTexture, extent);

        queue->submit({ commandEncoder->finish() });
    }

    // create font sampler
    {
        SamplerDescriptor fontSamplerDescriptor{};
        fontSamplerDescriptor.addressModeU = AddressMode::kClampToEdge;
        fontSamplerDescriptor.addressModeV = AddressMode::kClampToEdge;
        fontSamplerDescriptor.addressModeW = AddressMode::kClampToEdge;
        fontSamplerDescriptor.lodMin = 0.0f;
        fontSamplerDescriptor.lodMax = static_cast<uint32_t>(m_fontTexture->getMipLevels());
        fontSamplerDescriptor.minFilter = FilterMode::kLinear;
        fontSamplerDescriptor.magFilter = FilterMode::kLinear;
        fontSamplerDescriptor.mipmapFilter = MipmapFilterMode::kLinear;

        m_fontSampler = device->createSampler(fontSamplerDescriptor);
    }

    // create binding group layout
    {
        SamplerBindingLayout fontSamplerBindingLayout{};
        fontSamplerBindingLayout.index = 0;
        fontSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
        fontSamplerBindingLayout.withTexture = m_fontTextureView != nullptr;

        BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
        bindingGroupLayoutDescriptor.samplers = { fontSamplerBindingLayout };

        m_bindingGroupLayout = device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
    }

    // create binding group
    {
        SamplerBinding fontSamplerBinding{};
        fontSamplerBinding.index = 0;
        fontSamplerBinding.sampler = m_fontSampler.get();
        fontSamplerBinding.textureView = m_fontTextureView.get();

        BindingGroupDescriptor bindingGroupDescriptor{};
        bindingGroupDescriptor.layout = m_bindingGroupLayout.get();
        bindingGroupDescriptor.samplers = { fontSamplerBinding };

        m_bindingGroup = device->createBindingGroup(bindingGroupDescriptor);
    }

    // create pipeline layout
    {
        PipelineLayoutDescriptor pipelineLayoutDescriptor{};
        pipelineLayoutDescriptor.layouts = { m_bindingGroupLayout.get() };

        m_pipelineLayout = device->createPipelineLayout(pipelineLayoutDescriptor);
    }

    // create pipeline
    {
        InputAssemblyStage inputAssemblyStage{};
        {
            inputAssemblyStage.topology = PrimitiveTopology::kTriangleList;
        }

        std::unique_ptr<ShaderModule> vertexShaderModule = nullptr;
        VertexStage vertexStage{};
        {

            VertexInputLayout vertexInputLayout{};
            {
                VertexAttribute positionAttribute{};
                positionAttribute.format = VertexFormat::kSFLOATx2;
                positionAttribute.offset = offsetof(ImDrawVert, pos);

                VertexAttribute uiAttribute{};
                uiAttribute.format = VertexFormat::kSFLOATx2;
                uiAttribute.offset = offsetof(ImDrawVert, uv);

                VertexAttribute colorAttribute{};
                colorAttribute.format = VertexFormat::kUNORM8x4;
                colorAttribute.offset = offsetof(ImDrawVert, col);

                vertexInputLayout.attributes = { positionAttribute, uiAttribute, colorAttribute };
                vertexInputLayout.mode = VertexMode::kVertex;
                vertexInputLayout.stride = sizeof(ImDrawVert);
            }

            {
                ShaderModuleDescriptor vertexShaderModuleDescriptor{};
                vertexShaderModuleDescriptor.code = reinterpret_cast<const char*>(vertexShaderSourceSpv.data());
                vertexShaderModuleDescriptor.codeSize = static_cast<uint32_t>(vertexShaderSourceSpv.size() * 4);

                vertexShaderModule = device->createShaderModule(vertexShaderModuleDescriptor);
            }

            vertexStage.entryPoint = "main";
            vertexStage.layouts = { vertexInputLayout };
            vertexStage.shaderModule = vertexShaderModule.get();
        }

        RasterizationStage rasterizationStage{};
        {
            rasterizationStage.cullMode = CullMode::kNone;
            rasterizationStage.frontFace = FrontFace::kCounterClockwise;
            rasterizationStage.sampleCount = 1;
        }

        std::unique_ptr<ShaderModule> fragmentShaderModule = nullptr;
        FragmentStage fragmentStage{};
        {
            FragmentStage::Target target{};
            target.format = swapchain->getTextureFormat();
            target.blend = {
                .color = {
                    .srcFactor = BlendFactor::kSrcAlpha,
                    .dstFactor = BlendFactor::kOneMinusSrcAlpha,
                    .operation = BlendOperation::kAdd,
                },
                .alpha = {

                    .srcFactor = BlendFactor::kOneMinusSrcAlpha,
                    .dstFactor = BlendFactor::kZero,
                    .operation = BlendOperation::kAdd,
                }
            };

            {
                ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
                fragmentShaderModuleDescriptor.code = reinterpret_cast<const char*>(fragmentShaderSourceSpv.data());
                fragmentShaderModuleDescriptor.codeSize = static_cast<uint32_t>(fragmentShaderSourceSpv.size() * 4);

                fragmentShaderModule = device->createShaderModule(fragmentShaderModuleDescriptor);
            }

            fragmentStage.targets = { target };
            fragmentStage.entryPoint = "main";
            fragmentStage.shaderModule = fragmentShaderModule.get();
        }

        RenderPipelineDescriptor renderPipelineDescriptor{};
        renderPipelineDescriptor.layout = m_pipelineLayout.get();
        renderPipelineDescriptor.inputAssembly = inputAssemblyStage;
        renderPipelineDescriptor.vertex = vertexStage;
        renderPipelineDescriptor.rasterization = rasterizationStage;
        renderPipelineDescriptor.fragment = fragmentStage;

        m_pipeline = device->createRenderPipeline(renderPipelineDescriptor);
    } // namespace vkt
}

void Im_Gui::updateImGui()
{

    // update buffer
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();
        uint32_t vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
        uint32_t indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

        if ((vertexBufferSize == 0) || (indexBufferSize == 0))
        {
            return;
        }

        // Update buffers only if vertex or index count has been changed compared to current buffer size

        // Vertex buffer
        if ((m_vertexBuffer == nullptr) || (m_vertexBuffer->getSize() != vertexBufferSize))
        {
            m_vertexBuffer.reset();

            BufferDescriptor descriptor{};
            descriptor.size = vertexBufferSize;
            descriptor.usage = BufferUsageFlagBits::kVertex;

            m_vertexBuffer = m_device->createBuffer(descriptor);
            m_vertexBuffer->map();
        }

        // Index buffer
        if ((m_indexBuffer == nullptr) || (m_indexBuffer->getSize() < indexBufferSize))
        {
            m_indexBuffer.reset();

            BufferDescriptor descriptor{};
            descriptor.size = indexBufferSize;
            descriptor.usage = BufferUsageFlagBits::kIndex;

            m_indexBuffer = m_device->createBuffer(descriptor);
            m_indexBuffer->map();
        }

        // Upload data
        ImDrawVert* vtxDst = (ImDrawVert*)m_vertexBuffer->map();
        ImDrawIdx* idxDst = (ImDrawIdx*)m_indexBuffer->map();

        for (int n = 0; n < imDrawData->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[n];
            memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtxDst += cmd_list->VtxBuffer.Size;
            idxDst += cmd_list->IdxBuffer.Size;
        }

        // Flush if need
    }
}

void Im_Gui::drawImGui(RenderPassEncoder* renderPassEncoder)
{
    ImDrawData* imDrawData = ImGui::GetDrawData();
    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    ImGuiIO& io = ImGui::GetIO();

    renderPassEncoder->setPipeline(m_pipeline.get());
    renderPassEncoder->setBindingGroup(0, m_bindingGroup.get());
    renderPassEncoder->setViewport(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, 0, 1);

    if (imDrawData->CmdListsCount > 0)
    {
        renderPassEncoder->setVertexBuffer(m_vertexBuffer.get());
        renderPassEncoder->setIndexBuffer(m_indexBuffer.get());

        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                auto scissorX = std::max((int32_t)(pcmd->ClipRect.x), 0);
                auto scissorY = std::max((int32_t)(pcmd->ClipRect.y), 0);
                auto scissorWidth = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                auto scissorHeight = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
                renderPassEncoder->setScissor(scissorX, scissorY, scissorWidth, scissorHeight);
                renderPassEncoder->drawIndexed(pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
                indexOffset += pcmd->ElemCount;
            }
            vertexOffset += cmd_list->VtxBuffer.Size;
        }
    }
}

void Im_Gui::clearImGui()
{
    ImGui::DestroyContext();

    m_fontSampler.reset();
    m_fontTextureView.reset();
    m_fontTexture.reset();

    m_vertexBuffer.reset();
    m_indexBuffer.reset();

    m_pipeline.reset();
    m_pipelineLayout.reset();
    m_bindingGroup.reset();
    m_bindingGroupLayout.reset();
}

} // namespace vkt