#pragma once

#include "vulkan_resource.h"

namespace jipu
{

struct VulkanResourceAllocatorDescriptor
{
};

class VulkanDevice;
class VulkanResourceAllocator final
{
public:
    VulkanResourceAllocator() = delete;
    VulkanResourceAllocator(VulkanDevice* device, const VulkanResourceAllocatorDescriptor& descriptor);
    ~VulkanResourceAllocator();

    VulkanBufferResource createBuffer(const VkBufferCreateInfo& createInfo);
    void destroyBuffer(const VulkanBufferResource& bufferMemory);

    VulkanTextureResource createTexture(const VkImageCreateInfo& createInfo);
    void destroyTexture(VulkanTextureResource textureMemory);

    void* map(VmaAllocation allocation);
    void unmap(VmaAllocation allocation);

private:
    VulkanDevice* m_device = nullptr;
    VmaAllocator m_allocator = VK_NULL_HANDLE;
    VmaVulkanFunctions m_vmaFunctions{};
};

} // namespace jipu