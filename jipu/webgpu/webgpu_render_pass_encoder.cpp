#include "webgpu_render_pass_encoder.h"

#include "webgpu_command_encoder.h"
#include "webgpu_render_pipeline.h"
#include "webgpu_texture_view.h"

namespace jipu
{

WebGPURenderPassEncoder* WebGPURenderPassEncoder::create(WebGPUCommandEncoder* wgpuCommandEncoder, WGPURenderPassDescriptor const* descriptor)
{
    auto commandEncoder = wgpuCommandEncoder->getCommandEncoder();

    RenderPassEncoderDescriptor renderPassEncoderDescriptor{};

    // RenderPassEncoderDescriptor: colorAttachments
    {
        if (descriptor->colorAttachmentCount <= 0)
        {
            throw std::runtime_error("colorAttachmentCount must be greater than 0");
        }

        for (auto i = 0; i < descriptor->colorAttachmentCount; i++)
        {
            auto wgpuColorAttachment = descriptor->colorAttachments[i];

            ColorAttachment colorAttachment;
            colorAttachment.clearValue = ToColor(wgpuColorAttachment.clearValue);
            colorAttachment.renderView = reinterpret_cast<WebGPUTextureView*>(wgpuColorAttachment.view)->getTextureView();
            colorAttachment.loadOp = ToLoadOp(wgpuColorAttachment.loadOp);
            colorAttachment.storeOp = ToStoreOp(wgpuColorAttachment.storeOp);

            auto resolveTarget = reinterpret_cast<WebGPUTextureView*>(wgpuColorAttachment.resolveTarget);
            if (resolveTarget)
            {
                colorAttachment.resolveView = resolveTarget->getTextureView();
            }

            renderPassEncoderDescriptor.colorAttachments.push_back(colorAttachment);
        }
    }

    // RenderPassEncoderDescriptor: depthStencilAttachment
    {
        if (descriptor->depthStencilAttachment)
        {
            const auto wgpuDepthStencilAttachment = descriptor->depthStencilAttachment;

            DepthStencilAttachment depthStencilAttachment;
            depthStencilAttachment.clearValue.depth = wgpuDepthStencilAttachment->depthClearValue;
            depthStencilAttachment.clearValue.stencil = wgpuDepthStencilAttachment->stencilClearValue;
            depthStencilAttachment.textureView = reinterpret_cast<WebGPUTextureView*>(wgpuDepthStencilAttachment->view)->getTextureView();
            depthStencilAttachment.depthLoadOp = ToLoadOp(wgpuDepthStencilAttachment->depthLoadOp);
            depthStencilAttachment.depthStoreOp = ToStoreOp(wgpuDepthStencilAttachment->depthStoreOp);
            depthStencilAttachment.stencilLoadOp = ToLoadOp(wgpuDepthStencilAttachment->stencilLoadOp);
            depthStencilAttachment.stencilStoreOp = ToStoreOp(wgpuDepthStencilAttachment->stencilStoreOp);

            renderPassEncoderDescriptor.depthStencilAttachment = depthStencilAttachment;
        }
    }

    // TODO: occlusionQuerySet
    // TODO: timestampWrites
    auto renderPassEncoder = commandEncoder->beginRenderPass(renderPassEncoderDescriptor);

    return new WebGPURenderPassEncoder(wgpuCommandEncoder, std::move(renderPassEncoder), descriptor);
}

WebGPURenderPassEncoder::WebGPURenderPassEncoder(WebGPUCommandEncoder* wgpuCommandEncoder, std::unique_ptr<RenderPassEncoder> renderPassEncoder, WGPURenderPassDescriptor const* descriptor)
    : m_wgpuCommandEncoder(wgpuCommandEncoder)
    , m_descriptor(*descriptor)
    , m_renderPassEncoder(std::move(renderPassEncoder))
{
}

void WebGPURenderPassEncoder::setPipeline(WGPURenderPipeline wgpuPipeline)
{
    auto renderPipeline = reinterpret_cast<WebGPURenderPipeline*>(wgpuPipeline)->getRenderPipeline();
    m_renderPassEncoder->setPipeline(renderPipeline);

    // TODO: default viewport
    {
        auto textureView = reinterpret_cast<WebGPUTextureView*>(m_descriptor.colorAttachments[0].view)->getTextureView();
        auto width = textureView->getWidth();
        auto height = textureView->getHeight();
        m_renderPassEncoder->setViewport(0, 0, width, height, 0, 1);
    }

    // TODO: default scissor
    {
        auto textureView = reinterpret_cast<WebGPUTextureView*>(m_descriptor.colorAttachments[0].view)->getTextureView();
        auto width = textureView->getWidth();
        auto height = textureView->getHeight();
        m_renderPassEncoder->setScissor(0, 0, width, height);
    }
}

void WebGPURenderPassEncoder::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    m_renderPassEncoder->draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void WebGPURenderPassEncoder::end()
{
    m_renderPassEncoder->end();
}

RenderPassEncoder* WebGPURenderPassEncoder::getRenderPassEncoder() const
{
    return m_renderPassEncoder.get();
}

// Convert from WebGPU to JIPU
WGPUColor ToWGPUColor(Color color)
{
    WGPUColor wgpuColor{};
    wgpuColor.r = color.r;
    wgpuColor.g = color.g;
    wgpuColor.b = color.b;
    wgpuColor.a = color.a;

    return wgpuColor;
}

WGPULoadOp ToWGPULoadOp(LoadOp loadOp)
{
    switch (loadOp)
    {
    case LoadOp::kDontCare:
        return WGPULoadOp_Undefined;
    case LoadOp::kLoad:
        return WGPULoadOp_Load;
    case LoadOp::kClear:
        return WGPULoadOp_Clear;
    default:
        return WGPULoadOp_Undefined;
    }
}

WGPUStoreOp ToWGPUStoreOp(StoreOp storeOp)
{
    switch (storeOp)
    {
    case StoreOp::kDontCare:
        return WGPUStoreOp_Undefined;
    case StoreOp::kStore:
        return WGPUStoreOp_Store;
    default:
        return WGPUStoreOp_Undefined;
    }
}

// Convert from JIPU to WebGPU
Color ToColor(WGPUColor color)
{
    Color jipuColor{};
    jipuColor.r = color.r;
    jipuColor.g = color.g;
    jipuColor.b = color.b;
    jipuColor.a = color.a;

    return jipuColor;
}

LoadOp ToLoadOp(WGPULoadOp loadOp)
{
    switch (loadOp)
    {
    case WGPULoadOp_Undefined:
        return LoadOp::kDontCare;
    case WGPULoadOp_Load:
        return LoadOp::kLoad;
    case WGPULoadOp_Clear:
        return LoadOp::kClear;
    default:
        return LoadOp::kDontCare;
    }
}

StoreOp ToStoreOp(WGPUStoreOp storeOp)
{
    switch (storeOp)
    {
    case WGPUStoreOp_Undefined:
        return StoreOp::kDontCare;
    case WGPUStoreOp_Store:
        return StoreOp::kStore;
    default:
        return StoreOp::kDontCare;
    }
}

} // namespace jipu