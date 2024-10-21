#include "webgpu_buffer.h"

namespace jipu
{

WebGPUBuffer* WebGPUBuffer::create(WebGPUDevice* device, WGPUBufferDescriptor const* descriptor)
{
    return new WebGPUBuffer(device, nullptr, descriptor);
}

WebGPUBuffer::WebGPUBuffer(WebGPUDevice* device, std::unique_ptr<Buffer> buffer, WGPUBufferDescriptor const* descriptor)
    : m_wgpuDevice(device)
    , m_descriptor(*descriptor)
    , m_buffer(std::move(buffer))
{
}

Buffer* WebGPUBuffer::getBuffer() const
{
    return m_buffer.get();
}

} // namespace jipu