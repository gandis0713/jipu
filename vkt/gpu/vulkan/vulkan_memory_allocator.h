#pragma once

namespace vkt
{

struct VulkanMemoryAllocatorDescriptor
{
};

class VulkanDevice;
class VulkanMemoryAllocator final
{
public:
    VulkanMemoryAllocator() = delete;
    VulkanMemoryAllocator(VulkanDevice* device, const VulkanMemoryAllocatorDescriptor& descriptor);
    ~VulkanMemoryAllocator();

private:
    VulkanDevice* m_device;
};

} // namespace vkt