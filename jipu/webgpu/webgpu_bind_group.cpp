#include "webgpu_bind_group.h"

#include "webgpu_bind_group_layout.h"
#include "webgpu_buffer.h"
#include "webgpu_device.h"
#include "webgpu_sampler.h"
#include "webgpu_texture_view.h"

namespace jipu
{

WebGPUBindGroup* WebGPUBindGroup::create(WebGPUDevice* wgpuDevice, WGPUBindGroupDescriptor const* descriptor)
{
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
            bindingGroupDescriptor.samplers.push_back(SamplerBinding{
                .index = entry.binding,
                .sampler = reinterpret_cast<WebGPUSampler*>(entry.sampler)->getSampler(),
            });
        }

        if (entry.textureView)
        {
            bindingGroupDescriptor.textures.push_back(TextureBinding{
                .index = entry.binding,
                .textureView = reinterpret_cast<WebGPUTextureView*>(entry.textureView)->getTextureView(),
            });
        }
    }

    auto device = wgpuDevice->getDevice();
    auto bindingGroup = device->createBindingGroup(bindingGroupDescriptor);

    return new WebGPUBindGroup(wgpuDevice, std::move(bindingGroup), descriptor);
}

WebGPUBindGroup::WebGPUBindGroup(WebGPUDevice* wgpuDevice, std::unique_ptr<BindingGroup> bindingGroup, WGPUBindGroupDescriptor const* descriptor)
    : m_wgpuDevice(wgpuDevice)
    , m_descriptor(*descriptor)
    , m_bindingGroup(std::move(bindingGroup))
{
}

BindingGroup* WebGPUBindGroup::getBindingGroup() const
{
    return m_bindingGroup.get();
}

} // namespace jipu