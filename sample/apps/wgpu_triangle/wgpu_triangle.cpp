#include "wgpu_triangle.h"

#include <spdlog/spdlog.h>

namespace jipu
{

WGPUTriangleSample::WGPUTriangleSample(const WGPUSampleDescriptor& descriptor)
    : WGPUSample(descriptor)
{
}

WGPUTriangleSample::~WGPUTriangleSample()
{
    wgpuRenderPipelineRelease(m_renderPipeline);
    wgpuPipelineLayoutRelease(m_pipelineLayout);
    wgpuShaderModuleRelease(m_vertexShaderModule);
    wgpuShaderModuleRelease(m_fragShaderModule);
}

void WGPUTriangleSample::init()
{
    WGPUSample::init();

    createShaderModule();
    createPipelineLayout();
    createPipeline();
}

void WGPUTriangleSample::update()
{
    WGPUSample::update();
}

void WGPUTriangleSample::draw()
{
    WGPUSurfaceTexture surfaceTexture{};
    wgpuSurfaceGetCurrentTexture(m_wgpuContext.surface, &surfaceTexture);

    WGPUTextureView surfaceTextureView = wgpuTextureCreateView(surfaceTexture.texture, NULL);

    WGPUCommandEncoderDescriptor commandEncoderDescriptor{};
    WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(m_wgpuContext.device, &commandEncoderDescriptor);

    WGPURenderPassColorAttachment colorAttachment{};
    colorAttachment.view = surfaceTextureView;
    colorAttachment.loadOp = WGPULoadOp_Clear;
    colorAttachment.storeOp = WGPUStoreOp_Store;
    colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    colorAttachment.clearValue = { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };

    WGPURenderPassDescriptor renderPassDescriptor{};
    renderPassDescriptor.colorAttachmentCount = 1;
    renderPassDescriptor.colorAttachments = &colorAttachment;

    WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);

    wgpuRenderPassEncoderSetPipeline(renderPassEncoder, m_renderPipeline);
    wgpuRenderPassEncoderDraw(renderPassEncoder, 3, 1, 0, 0);
    wgpuRenderPassEncoderEnd(renderPassEncoder);
    wgpuRenderPassEncoderRelease(renderPassEncoder);

    WGPUCommandBufferDescriptor commandBufferDescriptor{};
    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);

    wgpuQueueSubmit(m_wgpuContext.queue, 1, &commandBuffer);
    wgpuSurfacePresent(m_wgpuContext.surface);

    wgpuCommandBufferRelease(commandBuffer);
    wgpuCommandEncoderRelease(commandEncoder);
    wgpuTextureViewRelease(surfaceTextureView);
    wgpuTextureRelease(surfaceTexture.texture);
}

void WGPUTriangleSample::createShaderModule()
{
    {
        const char* vertexShaderCode = R"(
        @vertex
        fn main(@location(0) position: vec3<f32>) -> @builtin(position) vec4<f32> {
            return vec4<f32>(position, 1.0);
        }
    )";

        const char* fragmentShaderCode = R"(
        @fragment
        fn main() -> @location(0) vec4<f32> {
            return vec4<f32>(1.0, 0.0, 0.0, 1.0); // 빨간색으로 출력
        }
    )";

        WGPUShaderModuleWGSLDescriptor vertexShaderModuleWGSLDescriptor{};
        vertexShaderModuleWGSLDescriptor.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        vertexShaderModuleWGSLDescriptor.code = vertexShaderCode;

        WGPUShaderModuleDescriptor vertexShaderModuleDescriptor{};
        vertexShaderModuleDescriptor.nextInChain = &vertexShaderModuleWGSLDescriptor.chain;

        m_vertexShaderModule = wgpuDeviceCreateShaderModule(m_wgpuContext.device, &vertexShaderModuleDescriptor);

        WGPUShaderModuleWGSLDescriptor fragShaderModuleWGSLDescriptor{};
        fragShaderModuleWGSLDescriptor.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        fragShaderModuleWGSLDescriptor.code = fragmentShaderCode;

        WGPUShaderModuleDescriptor fragShaderModuleDescriptor{};
        fragShaderModuleDescriptor.nextInChain = &fragShaderModuleWGSLDescriptor.chain;

        m_fragShaderModule = wgpuDeviceCreateShaderModule(m_wgpuContext.device, &fragShaderModuleDescriptor);
    }

    const char* shaderCode = R"(
        @vertex
        fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4<f32> {
            let x = f32(i32(in_vertex_index) - 1);
            let y = f32(i32(in_vertex_index & 1u) * 2 - 1);
            return vec4<f32>(x, y, 0.0, 1.0);
        }

        @fragment
        fn fs_main() -> @location(0) vec4<f32> {
            return vec4<f32>(1.0, 0.0, 0.0, 1.0);
        }
    )";

    WGPUShaderModuleWGSLDescriptor shaderModuleWGSLDescriptor{};
    shaderModuleWGSLDescriptor.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    shaderModuleWGSLDescriptor.code = shaderCode;

    WGPUShaderModuleDescriptor shaderModuleDescriptor{};
    shaderModuleDescriptor.nextInChain = &shaderModuleWGSLDescriptor.chain;

    m_shaderModule = wgpuDeviceCreateShaderModule(m_wgpuContext.device, &shaderModuleDescriptor);
}

void WGPUTriangleSample::createPipelineLayout()
{
    WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor{};
    m_pipelineLayout = wgpuDeviceCreatePipelineLayout(m_wgpuContext.device, &pipelineLayoutDescriptor);
}

void WGPUTriangleSample::createPipeline()
{

    WGPUPrimitiveState primitiveState{};
    primitiveState.topology = WGPUPrimitiveTopology_TriangleList;
    primitiveState.cullMode = WGPUCullMode_None;
    primitiveState.frontFace = WGPUFrontFace_CCW;
    // primitiveState.stripIndexFormat = WGPUIndexFormat_Undefined;

    WGPUVertexState vertexState{};
    vertexState.entryPoint = "vs_main";
    vertexState.module = m_shaderModule;

    WGPUColorTargetState colorTargetState{};
    colorTargetState.format = m_wgpuContext.surfaceCapabilities.formats[0];
    colorTargetState.writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragState{};
    fragState.entryPoint = "fs_main";
    fragState.module = m_shaderModule;
    fragState.targetCount = 1;
    fragState.targets = &colorTargetState;

    // WGPUDepthStencilState depthStencilState{};
    // depthStencilState.format = WGPUTextureFormat_Depth24PlusStencil8;

    WGPUMultisampleState multisampleState{};
    multisampleState.count = 1;
    multisampleState.mask = 0xFFFFFFFF;

    WGPURenderPipelineDescriptor renderPipelineDescriptor{};
    renderPipelineDescriptor.layout = m_pipelineLayout;
    renderPipelineDescriptor.primitive = primitiveState;
    renderPipelineDescriptor.multisample = multisampleState;
    renderPipelineDescriptor.vertex = vertexState;
    renderPipelineDescriptor.fragment = &fragState;

    m_renderPipeline = wgpuDeviceCreateRenderPipeline(m_wgpuContext.device, &renderPipelineDescriptor);
}

} // namespace jipu