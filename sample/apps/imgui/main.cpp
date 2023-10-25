

#include "file.h"
#include "sample.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

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
#include <spdlog/spdlog.h>

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

class ImGuiSample : public Sample
{
public:
    ImGuiSample() = delete;
    ImGuiSample(const SampleDescriptor& descriptor);
    ~ImGuiSample() override;

    void init() override;
    void draw() override;

private:
    void initImGui();
    void updateImGui();
    void drawImGui(RenderPassEncoder* renderPassEncoder);
    void clearImGui();

private:
    void createDevier();
    void createPhysicalDevice();
    void createSurface();
    void createDevice();
    void createSwapchain();
    void createCommandBuffer();
    void createQueue();
    void createVertexBuffer();
    void createRenderPipeline();

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
    std::unique_ptr<CommandBuffer> m_commandBuffer = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;
    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    struct ImGuiResources
    {
        std::unique_ptr<Buffer> vertexBuffer = nullptr;
        std::unique_ptr<Buffer> indexBuffer = nullptr;
        std::unique_ptr<Texture> fontTexture = nullptr;
        std::unique_ptr<TextureView> fontTextureView = nullptr;
        std::unique_ptr<Sampler> fontSampler = nullptr;
        std::unique_ptr<BindingGroupLayout> bindingGroupLayout = nullptr;
        std::unique_ptr<BindingGroup> bindingGroup = nullptr;
        std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
        std::unique_ptr<Pipeline> pipeline = nullptr;
    } m_imguiResources{};

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
    m_physicalDevice.reset();
    m_driver.reset();
}

void ImGuiSample::init()
{
    createDevier();
    createPhysicalDevice();
    createSurface();
    createDevice();
    createSwapchain();
    createCommandBuffer();
    createQueue();
    createVertexBuffer();
    createRenderPipeline();

    initImGui();
}

void ImGuiSample::draw()
{

    auto swapchainIndex = m_swapchain->acquireNextTexture();

    updateImGui();

    {
        ColorAttachment attachment{};
        attachment.clearValue = { .float32 = { 0.0, 0.0, 0.0, 0.0 } };
        attachment.loadOp = LoadOp::kClear;
        attachment.storeOp = StoreOp::kStore;
        attachment.renderView = m_swapchain->getTextureView(swapchainIndex);
        attachment.resolveView = nullptr;

        RenderPassEncoderDescriptor renderPassDescriptor;
        renderPassDescriptor.sampleCount = m_sampleCount;
        renderPassDescriptor.colorAttachments = { attachment };

        CommandEncoderDescriptor commandDescriptor{};
        auto commadEncoder = m_commandBuffer->createCommandEncoder(commandDescriptor);

        auto renderPassEncoder = commadEncoder->beginRenderPass(renderPassDescriptor);
        renderPassEncoder->setPipeline(m_renderPipeline.get());
        renderPassEncoder->setVertexBuffer(m_vertexBuffer.get());
        renderPassEncoder->setScissor(0, 0, m_width, m_height);
        renderPassEncoder->setViewport(0, 0, m_width, m_height, 0, 1);
        renderPassEncoder->draw(static_cast<uint32_t>(m_vertices.size()));

        drawImGui(renderPassEncoder.get());

        renderPassEncoder->end();

        m_queue->submit({ commadEncoder->finish() }, m_swapchain.get());
    }
}

void ImGuiSample::initImGui()
{
    // IMGUI_CHECKVERSION();
    ImGuiContext* imguiContext = ImGui::CreateContext();
    if (imguiContext == nullptr)
    {
        throw std::runtime_error("Failed to create imgui context");
    }

    auto scale = 1.0f;
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = scale;
    io.DisplaySize = ImVec2(m_width, m_height);
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

        m_imguiResources.fontTexture = m_device->createTexture(fontTextureDescriptor);
    }

    // create font texture view.
    {
        TextureViewDescriptor fontTextureViewDescriptor{};
        fontTextureViewDescriptor.aspect = TextureAspectFlagBits::kColor;
        fontTextureViewDescriptor.type = TextureViewType::k2D;

        m_imguiResources.fontTextureView = m_imguiResources.fontTexture->createTextureView(fontTextureViewDescriptor);
    }

    // create font staging buffer.
    std::unique_ptr<Buffer> m_fontBuffer = nullptr;
    {
        BufferDescriptor fontBufferDescriptor{};
        fontBufferDescriptor.size = fontTexWidth * fontTexHeight * 4 * sizeof(FontDataType);
        fontBufferDescriptor.usage = BufferUsageFlagBits::kCopySrc;

        m_fontBuffer = m_device->createBuffer(fontBufferDescriptor);

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
        blitTextureBuffer.bytesPerRow = bytesPerData * m_imguiResources.fontTexture->getWidth() * channel;
        blitTextureBuffer.rowsPerTexture = m_imguiResources.fontTexture->getHeight();

        BlitTexture blitTexture{ .texture = m_imguiResources.fontTexture.get() };
        Extent3D extent{};
        extent.width = m_imguiResources.fontTexture->getWidth();
        extent.height = m_imguiResources.fontTexture->getHeight();
        extent.depth = 1;

        CommandBufferDescriptor commandBufferDescriptor{ .usage = CommandBufferUsage::kOneTime };
        std::unique_ptr<CommandBuffer> commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);

        CommandEncoderDescriptor commandEncoderDescriptor{};
        std::unique_ptr<CommandEncoder> commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);
        commandEncoder->copyBufferToTexture(blitTextureBuffer, blitTexture, extent);

        m_queue->submit({ commandEncoder->finish() });
    }

    // create font sampler
    {
        SamplerDescriptor fontSamplerDescriptor{};
        fontSamplerDescriptor.addressModeU = AddressMode::kClampToEdge;
        fontSamplerDescriptor.addressModeV = AddressMode::kClampToEdge;
        fontSamplerDescriptor.addressModeW = AddressMode::kClampToEdge;
        fontSamplerDescriptor.lodMin = 0.0f;
        fontSamplerDescriptor.lodMax = static_cast<uint32_t>(m_imguiResources.fontTexture->getMipLevels());
        fontSamplerDescriptor.minFilter = FilterMode::kLinear;
        fontSamplerDescriptor.magFilter = FilterMode::kLinear;
        fontSamplerDescriptor.mipmapFilter = MipmapFilterMode::kLinear;

        m_imguiResources.fontSampler = m_device->createSampler(fontSamplerDescriptor);
    }

    // create binding group layout
    {
        SamplerBindingLayout fontSamplerBindingLayout{};
        fontSamplerBindingLayout.index = 0;
        fontSamplerBindingLayout.stages = BindingStageFlagBits::kFragmentStage;
        fontSamplerBindingLayout.withTexture = m_imguiResources.fontTextureView != nullptr;

        BindingGroupLayoutDescriptor bindingGroupLayoutDescriptor{};
        bindingGroupLayoutDescriptor.samplers = { fontSamplerBindingLayout };

        m_imguiResources.bindingGroupLayout = m_device->createBindingGroupLayout(bindingGroupLayoutDescriptor);
    }

    // create binding group
    {
        SamplerBinding fontSamplerBinding{};
        fontSamplerBinding.index = 0;
        fontSamplerBinding.sampler = m_imguiResources.fontSampler.get();
        fontSamplerBinding.textureView = m_imguiResources.fontTextureView.get();

        BindingGroupDescriptor bindingGroupDescriptor{};
        bindingGroupDescriptor.layout = m_imguiResources.bindingGroupLayout.get();
        bindingGroupDescriptor.samplers = { fontSamplerBinding };

        m_imguiResources.bindingGroup = m_device->createBindingGroup(bindingGroupDescriptor);
    }

    // create pipeline layout
    {
        PipelineLayoutDescriptor pipelineLayoutDescriptor{};
        pipelineLayoutDescriptor.layouts = { m_imguiResources.bindingGroupLayout.get() };

        m_imguiResources.pipelineLayout = m_device->createPipelineLayout(pipelineLayoutDescriptor);
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
                std::vector<char> vertexShaderSource = utils::readFile(m_appDir / "imgui.vert.spv", m_handle);
                ShaderModuleDescriptor vertexShaderModuleDescriptor{};
                vertexShaderModuleDescriptor.code = vertexShaderSource.data();
                vertexShaderModuleDescriptor.codeSize = static_cast<uint32_t>(vertexShaderSource.size());

                vertexShaderModule = m_device->createShaderModule(vertexShaderModuleDescriptor);
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
            target.format = m_swapchain->getTextureFormat();

            {
                std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "imgui.frag.spv", m_handle);
                ShaderModuleDescriptor fragmentShaderModuleDescriptor{};
                fragmentShaderModuleDescriptor.code = fragmentShaderSource.data();
                fragmentShaderModuleDescriptor.codeSize = static_cast<uint32_t>(fragmentShaderSource.size());

                fragmentShaderModule = m_device->createShaderModule(fragmentShaderModuleDescriptor);
            }

            fragmentStage.targets = { target };
            fragmentStage.entryPoint = "main";
            fragmentStage.shaderModule = fragmentShaderModule.get();
        }

        RenderPipelineDescriptor renderPipelineDescriptor{};
        renderPipelineDescriptor.layout = m_imguiResources.pipelineLayout.get();
        renderPipelineDescriptor.inputAssembly = inputAssemblyStage;
        renderPipelineDescriptor.vertex = vertexStage;
        renderPipelineDescriptor.rasterization = rasterizationStage;
        renderPipelineDescriptor.fragment = fragmentStage;

        m_imguiResources.pipeline = m_device->createRenderPipeline(renderPipelineDescriptor);
    }
}
void ImGuiSample::updateImGui()
{
    // TODO: draw
    ImGui::NewFrame();

    auto scale = 1.0f;
    ImGui::SetNextWindowPos(ImVec2(20 * scale, 300 * scale), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300 * scale, 100 * scale), ImGuiCond_FirstUseEver);
    ImGui::Begin("Test");
    ImGui::Text("Hello, world %d", 123);
    ImGui::End();

    // SRS - ShowDemoWindow() sets its own initial position and size, cannot override here
    // ImGui::ShowDemoWindow();

    // Render to generate draw buffers
    ImGui::Render();

    // update buffer
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();
        VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
        VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

        if ((vertexBufferSize == 0) || (indexBufferSize == 0))
        {
            return;
        }

        // Update buffers only if vertex or index count has been changed compared to current buffer size

        // Vertex buffer
        if ((m_imguiResources.vertexBuffer == nullptr) || (m_imguiResources.vertexBuffer->getSize() != vertexBufferSize))
        {
            m_imguiResources.vertexBuffer.reset();

            BufferDescriptor descriptor{};
            descriptor.size = vertexBufferSize;
            descriptor.usage = BufferUsageFlagBits::kVertex;

            m_imguiResources.vertexBuffer = m_device->createBuffer(descriptor);
            m_imguiResources.vertexBuffer->map();
        }

        // Index buffer
        if ((m_imguiResources.indexBuffer == nullptr) || (m_imguiResources.indexBuffer->getSize() < indexBufferSize))
        {
            m_imguiResources.indexBuffer.reset();

            BufferDescriptor descriptor{};
            descriptor.size = indexBufferSize;
            descriptor.usage = BufferUsageFlagBits::kIndex;

            m_imguiResources.indexBuffer = m_device->createBuffer(descriptor);
            m_imguiResources.indexBuffer->map();
        }

        // Upload data
        ImDrawVert* vtxDst = (ImDrawVert*)m_imguiResources.vertexBuffer->map();
        ImDrawIdx* idxDst = (ImDrawIdx*)m_imguiResources.indexBuffer->map();

        for (int n = 0; n < imDrawData->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[n];
            memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtxDst += cmd_list->VtxBuffer.Size;
            idxDst += cmd_list->IdxBuffer.Size;
        }

        // Flush to make writes visible to GPU
        // vertexBuffer.flush();
        // indexBuffer.flush();
    }
}

void ImGuiSample::drawImGui(RenderPassEncoder* renderPassEncoder)
{
    ImDrawData* imDrawData = ImGui::GetDrawData();
    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    ImGuiIO& io = ImGui::GetIO();

    renderPassEncoder->setPipeline(m_imguiResources.pipeline.get());
    renderPassEncoder->setBindingGroup(0, m_imguiResources.bindingGroup.get());
    renderPassEncoder->setViewport(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, 0, 1);

    if (imDrawData->CmdListsCount > 0)
    {
        renderPassEncoder->setVertexBuffer(m_imguiResources.vertexBuffer.get());
        renderPassEncoder->setIndexBuffer(m_imguiResources.indexBuffer.get());

        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                VkRect2D scissorRect;
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

void ImGuiSample::clearImGui()
{
    ImGui::DestroyContext();

    m_imguiResources.fontSampler.reset();
    m_imguiResources.fontTextureView.reset();
    m_imguiResources.fontTexture.reset();

    m_imguiResources.vertexBuffer.reset();
    m_imguiResources.indexBuffer.reset();

    m_imguiResources.pipeline.reset();
    m_imguiResources.pipelineLayout.reset();
    m_imguiResources.bindingGroup.reset();
    m_imguiResources.bindingGroupLayout.reset();
}

void ImGuiSample::createDevier()
{
    DriverDescriptor descriptor{};
    descriptor.type = DriverType::VULKAN;

    m_driver = Driver::create(descriptor);
}

void ImGuiSample::createPhysicalDevice()
{
    PhysicalDeviceDescriptor descriptor{};
    descriptor.index = 0; // TODO: select device.

    m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
}

void ImGuiSample::createDevice()
{
    SurfaceDescriptor descriptor{};
    descriptor.windowHandle = getWindowHandle();

    m_surface = m_driver->createSurface(descriptor);
}

void ImGuiSample::createSurface()
{
    DeviceDescriptor descriptor{};

    m_device = m_physicalDevice->createDevice(descriptor);
}

void ImGuiSample::createSwapchain()
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
    VertexStage vertexStage{};
    {
        VertexAttribute positionAttribute{};
        positionAttribute.format = VertexFormat::kSFLOATx3;
        positionAttribute.offset = offsetof(Vertex, pos);

        VertexAttribute colorAttribute{};
        colorAttribute.format = VertexFormat::kSFLOATx3;
        colorAttribute.offset = offsetof(Vertex, color);

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
        std::vector<char> fragmentShaderSource = utils::readFile(m_appDir / "triangle.frag.spv", m_handle);
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
    descriptor.layout = renderPipelineLayout.get();

    m_renderPipeline = m_device->createRenderPipeline(descriptor);
}

} // namespace vkt

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Triangle", app },
        ""
    };

    vkt::ImGuiSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Triangle", nullptr },
        argv[0]
    };

    vkt::ImGuiSample sample(descriptor);

    return sample.exec();
}

#endif
