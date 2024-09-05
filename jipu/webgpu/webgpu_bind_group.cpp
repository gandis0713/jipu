#include "webgpu_bind_group.h"
#include "webgpu_bind_group_layout.h"
#include "webgpu_buffer.h"

namespace jipu
{

WebGPUBindGroup* WebGPUBindGroup::create(WebGPUDevice* device, WGPUBindGroupDescriptor const* descriptor)
{
    // typedef struct WGPUBindGroupEntry
    // {
    //     WGPUChainedStruct const* nextInChain;
    //     uint32_t binding;
    //     WGPU_NULLABLE WGPUBuffer buffer;
    //     uint64_t offset;
    //     uint64_t size;
    //     WGPU_NULLABLE WGPUSampler sampler;
    //     WGPU_NULLABLE WGPUTextureView textureView;
    // } WGPUBindGroupEntry WGPU_STRUCTURE_ATTRIBUTE;

    BindingGroupDescriptor bindingGroupDescriptor{};
    bindingGroupDescriptor.layout = reinterpret_cast<WebGPUBindGroupLayout*>(descriptor->layout)->getBindingGroupLayout();

    for (auto i = 0; i < descriptor->entryCount; i++)
    {
        auto entry = descriptor->entries[i];
        if (entry.buffer)
        {
            bindingGroupDescriptor.buffers.push_back(BufferBinding{
                .index = entry.binding,
                .offset = entry.offset,
                .size = entry.size,
                .buffer = reinterpret_cast<WebGPUBuffer*>(entry.buffer)->getBuffer(),
            });
        }

        if (entry.sampler)
        {
        }

        if (entry.textureView)
        {
        }
    }

    return nullptr;
}

WebGPUBindGroup::WebGPUBindGroup(WebGPUDevice* device, std::unique_ptr<BindingGroup> layout, WGPUBindGroupDescriptor const* descriptor)
    : m_wgpuDevice(device)
    , m_descriptor(*descriptor)
    , m_layout(std::move(layout))
{
}

BindingGroup* WebGPUBindGroup::getBindingGroup() const
{
    return m_layout.get();
}

} // namespace jipu