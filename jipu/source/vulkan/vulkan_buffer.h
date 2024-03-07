#pragma once

#include "export.h"

#include "jipu/buffer.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_export.h"
#include "vulkan_resource.h"

#include <memory>

namespace jipu
{

class VulkanDevice;
class CommandBuffer;
class VULKAN_EXPORT VulkanBuffer : public Buffer
{
public:
    VulkanBuffer() = delete;
    VulkanBuffer(VulkanDevice* device, const BufferDescriptor& descriptor) noexcept(false);
    ~VulkanBuffer() override;

    void* map() override;
    void unmap() override;

    BufferUsageFlags getUsage() const override;
    uint64_t getSize() const override;

public:
    void setTransition(CommandBuffer* commandBuffer, VkPipelineStageFlags flags);

    VkBuffer getVkBuffer() const;

private:
    VulkanBufferResource m_resource;
    VkPipelineStageFlags m_stageFlags = 0u;

    void* m_mappedPtr = nullptr;

private:
    Device* m_device = nullptr;
    BufferDescriptor m_descriptor{};
};

DOWN_CAST(VulkanBuffer, Buffer);

// Convert Helper
VkAccessFlags ToVkAccessFlags(BufferUsageFlags flags);
VkBufferUsageFlags ToVkBufferUsageFlags(BufferUsageFlags usage);
VkPipelineStageFlags ToVkPipelineStageFlags(BufferUsageFlags usage);

// TODO: remove or remain.
// BufferUsageFlags ToBufferUsageFlags(VkAccessFlags vkflags);
// BufferUsageFlags ToBufferUsageFlags(VkBufferUsageFlags usages);
} // namespace jipu
