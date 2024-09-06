#include "webgpu_command_encoder.h"

#include "webgpu_device.h"

namespace jipu
{

WebGPUCommandEncoder* WebGPUCommandEncoder::create(WebGPUDevice* wgpuDevice, WGPUCommandEncoderDescriptor const* descriptor)
{
    CommandEncoderDescriptor commandEncoderDescriptor{};
    // TODO
    return new WebGPUCommandEncoder(wgpuDevice, nullptr, descriptor);
}

WebGPUCommandEncoder::WebGPUCommandEncoder(WebGPUDevice* wgpuDevice, std::unique_ptr<CommandEncoder> commandEncoder, WGPUCommandEncoderDescriptor const* descriptor)
    : m_wgpuDevice(wgpuDevice)
    , m_descriptor(*descriptor)
    , m_commandEncoder(std::move(commandEncoder))
{
}

CommandEncoder* WebGPUCommandEncoder::getCommandEncoder() const
{
    return m_commandEncoder.get();
}

} // namespace jipu