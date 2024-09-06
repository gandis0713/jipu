#include "webgpu_command_encoder.h"

#include "webgpu_device.h"

namespace jipu
{

WebGPUCommandEncoder* WebGPUCommandEncoder::create(WebGPUDevice* wgpuDevice, WGPUCommandEncoderDescriptor const* descriptor)
{
    auto device = wgpuDevice->getDevice();
    CommandBufferDescriptor commandBufferDescriptor{};
    auto commandBuffer = device->createCommandBuffer(commandBufferDescriptor);

    CommandEncoderDescriptor commandEncoderDescriptor{};
    auto commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);

    return new WebGPUCommandEncoder(wgpuDevice, std::move(commandEncoder), std::move(commandBuffer), descriptor);
}

WebGPUCommandEncoder::WebGPUCommandEncoder(WebGPUDevice* wgpuDevice, std::unique_ptr<CommandEncoder> commandEncoder, std::unique_ptr<CommandBuffer> commandBuffer, WGPUCommandEncoderDescriptor const* descriptor)
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