#define VMA_IMPLEMENTATION
#include "vulkan_resource_allocator.h"

#include "vulkan_buffer.h"
#include "vulkan_device.h"
#include "vulkan_driver.h"
#include "vulkan_physical_device.h"
#include "vulkan_texture.h"

#include <fmt/format.h>

namespace jipu
{

VulkanResourceAllocator::VulkanResourceAllocator(VulkanDevice* device, const VulkanResourceAllocatorDescriptor& descriptor)
    : m_device(device)
{
    auto vulkanPhysicalDevice = downcast(m_device->getPhysicalDevice());
    auto vulkanDriver = downcast(vulkanPhysicalDevice->getDriver());

    auto physicalDevice = vulkanPhysicalDevice->getVkPhysicalDevice();
    auto instance = vulkanDriver->getVkInstance();

#if defined(VMA_DYNAMIC_VULKAN_FUNCTIONS)
    m_vmaFunctions.vkGetInstanceProcAddr = vulkanDriver->vkAPI.GetInstanceProcAddr;
    m_vmaFunctions.vkGetDeviceProcAddr = m_device->vkAPI.GetDeviceProcAddr;
#else
    // TODO: set functions
#endif

    VmaAllocatorCreateInfo createInfo{};
    createInfo.instance = instance;
    createInfo.physicalDevice = physicalDevice;
    createInfo.device = m_device->getVkDevice();
    createInfo.vulkanApiVersion = vulkanDriver->getDriverInfo().apiVersion;
    createInfo.pVulkanFunctions = &m_vmaFunctions;

    VkResult result = vmaCreateAllocator(&createInfo, &m_allocator);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vma allocator");
    }
}

VulkanResourceAllocator::~VulkanResourceAllocator()
{
    vmaDestroyAllocator(m_allocator);
}

VulkanBufferResource VulkanResourceAllocator::createBuffer(const VkBufferCreateInfo& createInfo)
{
    // VkBufferCreateInfo bufferCreateInfo{};
    // bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // bufferCreateInfo.size = descriptor.size;
    // bufferCreateInfo.flags = 0;
    // bufferCreateInfo.usage = ToVkBufferUsageFlags(descriptor.usage);
    // bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    // allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    VkBuffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation;
    VkResult result = vmaCreateBuffer(m_allocator, &createInfo, &allocInfo, &buffer, &allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create buffer. error: {}", static_cast<int32_t>(result)));
    }

    return { .buffer = buffer, .allocation = allocation };
}

void VulkanResourceAllocator::destroyBuffer(const VulkanBufferResource& bufferMemory)
{
    vmaDestroyBuffer(m_allocator, bufferMemory.buffer, bufferMemory.allocation);
}

VulkanTextureResource VulkanResourceAllocator::createTexture(const VkImageCreateInfo& createInfo)
{
    // VkImageCreateInfo imageCreateInfo{};
    // imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    // imageCreateInfo.imageType = ToVkImageType(descriptor.type);
    // imageCreateInfo.format = ToVkFormat(descriptor.format);
    // imageCreateInfo.extent = { descriptor.width, descriptor.height, descriptor.depth };
    // imageCreateInfo.mipLevels = descriptor.mipLevels;
    // imageCreateInfo.arrayLayers = 1;
    // imageCreateInfo.samples = ToVkSampleCountFlagBits(descriptor.sampleCount);
    // imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    // imageCreateInfo.usage = ToVkImageUsageFlags(descriptor.usage);
    // imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkImage image = VK_NULL_HANDLE;
    VmaAllocation allocation;
    VkResult result = vmaCreateImage(m_allocator, &createInfo, &allocInfo, &image, &allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create image. error: {}", static_cast<int32_t>(result)));
    }

    return { .image = image, .allocation = allocation };
}

void VulkanResourceAllocator::destroyTexture(VulkanTextureResource textureMemory)
{
    vmaDestroyImage(m_allocator, textureMemory.image, textureMemory.allocation);
}

void* VulkanResourceAllocator::map(VmaAllocation allocation)
{
    void* data = nullptr;
    vmaMapMemory(m_allocator, allocation, &data);
    return data;
}

void VulkanResourceAllocator::unmap(VmaAllocation allocation)
{
    vmaUnmapMemory(m_allocator, allocation);
}

} // namespace jipu