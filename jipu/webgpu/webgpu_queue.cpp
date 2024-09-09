#include "webgpu_queue.h"

#include "webgpu_command_buffer.h"
#include "webgpu_device.h"

#include "jipu/queue.h"

namespace jipu
{

WebGPUQueue* WebGPUQueue::create(WebGPUDevice* wgpuDevice)
{
    auto device = wgpuDevice->getDevice();
    auto queue = device->createQueue(QueueDescriptor{ .flags = QueueFlagBits::kGraphics |
                                                               QueueFlagBits::kCompute |
                                                               QueueFlagBits::kTransfer });
    return new WebGPUQueue(wgpuDevice, std::move(queue));
}

WebGPUQueue::WebGPUQueue(WebGPUDevice* wgpuDevice, std::unique_ptr<Queue> queue)
    : m_wgpuDevice(wgpuDevice)
    , m_queue(std::move(queue))
{
}

void WebGPUQueue::submit(size_t commandCount, WGPUCommandBuffer const* commands)
{
    std::vector<CommandBuffer*> commandBuffers{};

    for (auto i = 0; i < commandCount; ++i)
    {
        auto commandBuffer = reinterpret_cast<WebGPUCommandBuffer*>(commands[i])->getCommandBuffer();
        commandBuffers.push_back(commandBuffer);
    }

    m_queue->submit(commandBuffers);
}

} // namespace jipu