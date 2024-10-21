
#pragma once

#include "common/ref_counted.h"
#include "jipu/command_buffer.h"
#include "jipu/command_encoder.h"
#include "webgpu_header.h"

#include <memory>

namespace jipu
{

class WebGPUDevice;
class WebGPURenderPassEncoder;
class WebGPUCommandBuffer;
class WebGPUCommandEncoder : public RefCounted
{

public:
    static WebGPUCommandEncoder* create(WebGPUDevice* wgpuDevice, WGPUCommandEncoderDescriptor const* descriptor);

public:
    WebGPUCommandEncoder() = delete;
    explicit WebGPUCommandEncoder(WebGPUDevice* wgpuDevice, std::unique_ptr<CommandEncoder> commandEncoder, WGPUCommandEncoderDescriptor const* descriptor);

public:
    virtual ~WebGPUCommandEncoder() = default;

    WebGPUCommandEncoder(const WebGPUCommandEncoder&) = delete;
    WebGPUCommandEncoder& operator=(const WebGPUCommandEncoder&) = delete;

public: // WebGPU API
    WebGPURenderPassEncoder* beginRenderPass(WGPURenderPassDescriptor const* descriptor);
    WebGPUCommandBuffer* finish(WGPUCommandBufferDescriptor const* descriptor);

public:
    CommandEncoder* getCommandEncoder() const;

private:
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;
    [[maybe_unused]] const WGPUCommandEncoderDescriptor m_descriptor{};

private:
    std::unique_ptr<CommandEncoder> m_commandEncoder = nullptr;
};

} // namespace jipu