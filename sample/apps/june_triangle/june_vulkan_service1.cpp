#include "june_vulkan_service1.h"

#include <spdlog/spdlog.h>

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <android/sync.h>
#endif

namespace jipu
{

JuneVulkanService1::JuneVulkanService1(const JuneServiceDescriptor& descriptor)
    : JuneVulkanService(descriptor)
{
}

JuneVulkanService1::~JuneVulkanService1()
{
}

void JuneVulkanService1::begin()
{
    JuneVulkanService::begin();
}

void JuneVulkanService1::work()
{
}

JuneServiceShareObjects JuneVulkanService1::getSharingObject() const
{
    return m_sharingObjects;
}

void JuneVulkanService1::setSharedObjects(const JuneServiceShareObjects& sharedObjects)
{
    m_sharedObjects = sharedObjects;
    m_sharingObjects.sharedMemory = sharedObjects.sharedMemory;

    // Create ApiMemory and connect
    JuneApiMemory apiMemory{};
    {
        JuneApiMemoryDescriptor juneApiMemoryDescriptor{};
        juneApiMemoryDescriptor.nextInChain = nullptr;
        juneApiMemoryDescriptor.sharedMemory = m_sharedObjects.sharedMemory;

        apiMemory = m_juneAPI.ApiContextCreateApiMemory(m_juneApiContext, &juneApiMemoryDescriptor);
        m_sharingObjects.apiMemories.push_back(apiMemory);

        // for (const auto& sharedApiMemory : m_sharedObjects.apiMemories)
        // {
        //     m_juneAPI.ApiMemoryConnect(sharedApiMemory, m_juneApiMemory);
        //     m_juneAPI.ApiMemoryConnect(m_juneApiMemory, sharedApiMemory);
        // }
    }

    // Create Resource
    {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.extent.width = m_descriptor.width;
        imageInfo.extent.height = m_descriptor.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        JuneResourceVkImageDescriptor juneResourceVkImageDescriptor{};
        juneResourceVkImageDescriptor.chain.sType = JuneSType_VkImageResourceDescriptor;
        juneResourceVkImageDescriptor.vkImageCreateInfo = &imageInfo;

        JuneResourceDescriptor juneResourceDescriptor{};
        juneResourceDescriptor.nextInChain = &juneResourceVkImageDescriptor.chain;

        VkImage image = reinterpret_cast<VkImage>(m_juneAPI.ApiMemoryCreateResource(apiMemory, &juneResourceDescriptor));
        assert(image);
    }
}

} // namespace jipu
