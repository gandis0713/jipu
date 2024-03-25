#include "vulkan_device.h"

#include "vulkan_binding_group.h"
#include "vulkan_binding_group_layout.h"
#include "vulkan_buffer.h"
#include "vulkan_driver.h"
#include "vulkan_framebuffer.h"
#include "vulkan_physical_device.h"
#include "vulkan_queue.h"
#include "vulkan_sampler.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanDevice::VulkanDevice(VulkanPhysicalDevice& physicalDevice, const DeviceDescriptor& descriptor)
    : vkAPI(downcast(physicalDevice.getDriver()).vkAPI)
    , m_physicalDevice(physicalDevice)
    , m_renderPassCache(*this)
    , m_frameBufferCache(*this)
    , m_resourceAllocator(*this, {})
{
    const VulkanPhysicalDeviceInfo& info = physicalDevice.getVulkanPhysicalDeviceInfo();

    // GRAPHICS and COMPUTE imply TRANSFER
    constexpr uint32_t queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;

    std::unordered_set<uint32_t> queueFamilyIndices{};
    for (uint32_t i = 0; i < info.queueFamilyProperties.size(); ++i)
    {
        if ((info.queueFamilyProperties[i].queueFlags & queueFlags) == queueFlags)
        {
            queueFamilyIndices.insert(i);
        }
    }

    createDevice(queueFamilyIndices);

    const VulkanDeviceKnobs& deviceKnobs = static_cast<const VulkanDeviceKnobs&>(info);
    if (!vkAPI.loadDeviceProcs(m_device, deviceKnobs))
    {
        throw std::runtime_error("Failed to load device procs.");
    }

    // get queues.
    m_queues.resize(queueFamilyIndices.size());
    for (const uint32_t& index : queueFamilyIndices)
    {
        VkQueue queue{};
        vkAPI.GetDeviceQueue(m_device, index, 0, &queue);
        m_queues[index] = queue;
    }
}

VulkanDevice::~VulkanDevice()
{
    vkAPI.DeviceWaitIdle(m_device);

    vkAPI.DestroyCommandPool(m_device, m_commandPool, nullptr);
    vkAPI.DestroyDescriptorPool(m_device, m_descriptorPool, nullptr);

    m_frameBufferCache.clear();
    m_renderPassCache.clear();

    vkAPI.DestroyDevice(m_device, nullptr);
}

std::unique_ptr<Buffer> VulkanDevice::createBuffer(const BufferDescriptor& descriptor)
{
    return std::make_unique<VulkanBuffer>(*this, descriptor);
}

std::unique_ptr<BindingGroup> VulkanDevice::createBindingGroup(const BindingGroupDescriptor& descriptor)
{
    return std::make_unique<VulkanBindingGroup>(*this, descriptor);
}

std::unique_ptr<BindingGroupLayout> VulkanDevice::createBindingGroupLayout(const BindingGroupLayoutDescriptor& descriptor)
{
    return std::make_unique<VulkanBindingGroupLayout>(*this, descriptor);
}

std::unique_ptr<CommandBuffer> VulkanDevice::createCommandBuffer(const CommandBufferDescriptor& descriptor)
{
    return std::make_unique<VulkanCommandBuffer>(*this, descriptor);
}

std::unique_ptr<PipelineLayout> VulkanDevice::createPipelineLayout(const PipelineLayoutDescriptor& descriptor)
{
    return std::make_unique<VulkanPipelineLayout>(*this, descriptor);
}

std::unique_ptr<ComputePipeline> VulkanDevice::createComputePipeline(const ComputePipelineDescriptor& descriptor)
{
    return std::make_unique<VulkanComputePipeline>(*this, descriptor);
}

std::unique_ptr<RenderPipeline> VulkanDevice::createRenderPipeline(const RenderPipelineDescriptor& descriptor)
{
    return std::make_unique<VulkanRenderPipeline>(*this, descriptor);
}

std::unique_ptr<Queue> VulkanDevice::createQueue(const QueueDescriptor& descriptor)
{
    return std::make_unique<VulkanQueue>(*this, descriptor);
}

std::unique_ptr<Sampler> VulkanDevice::createSampler(const SamplerDescriptor& descriptor)
{
    return std::make_unique<VulkanSampler>(*this, descriptor);
}

std::unique_ptr<ShaderModule> VulkanDevice::createShaderModule(const ShaderModuleDescriptor& descriptor)
{
    return std::make_unique<VulkanShaderModule>(*this, descriptor);
}

std::unique_ptr<Swapchain> VulkanDevice::createSwapchain(const SwapchainDescriptor& descriptor)
{
    return std::make_unique<VulkanSwapchain>(*this, descriptor);
}

std::unique_ptr<Texture> VulkanDevice::createTexture(const TextureDescriptor& descriptor)
{
    return std::make_unique<VulkanTexture>(*this, descriptor);
}

std::unique_ptr<RenderPipeline> VulkanDevice::createRenderPipeline(const VulkanRenderPipelineDescriptor& descriptor)
{
    return std::make_unique<VulkanRenderPipeline>(*this, descriptor);
}

std::unique_ptr<BindingGroupLayout> VulkanDevice::createBindingGroupLayout(const VulkanBindingGroupLayoutDescriptor& descriptor)
{
    return std::make_unique<VulkanBindingGroupLayout>(*this, descriptor);
}

std::unique_ptr<Texture> VulkanDevice::createTexture(const VulkanTextureDescriptor& descriptor)
{
    return std::make_unique<VulkanTexture>(*this, descriptor);
}

std::unique_ptr<Swapchain> VulkanDevice::createSwapchain(const VulkanSwapchainDescriptor& descriptor)
{
    return std::make_unique<VulkanSwapchain>(*this, descriptor);
}

VulkanRenderPass& VulkanDevice::getRenderPass(const VulkanRenderPassDescriptor& descriptor)
{
    return m_renderPassCache.getRenderPass(descriptor);
}

VulkanFramebuffer& VulkanDevice::getFrameBuffer(const VulkanFramebufferDescriptor& descriptor)
{
    return m_frameBufferCache.getFrameBuffer(descriptor);
}

VulkanResourceAllocator& VulkanDevice::getResourceAllocator()
{
    return m_resourceAllocator;
}

VulkanPhysicalDevice& VulkanDevice::getPhysicalDevice() const
{
    return m_physicalDevice;
}

VkDevice VulkanDevice::getVkDevice() const
{
    return m_device;
}

VkPhysicalDevice VulkanDevice::getVkPhysicalDevice() const
{
    VulkanPhysicalDevice& vulkanPhysicalDevice = downcast(m_physicalDevice);

    return vulkanPhysicalDevice.getVkPhysicalDevice();
}

VkQueue VulkanDevice::getVkQueue(uint32_t index) const
{
    assert(m_queues.size() > index);

    // TODO: return suit queue
    return m_queues[index];
}

VkCommandPool VulkanDevice::getVkCommandPool()
{
    // TODO: get or create by command pool create information (not VkCommandPoolCreateInfo).
    // for instance, use queue index.
    if (m_commandPool == VK_NULL_HANDLE)
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = 0;                                    // TODO: get queue index by create information.
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

        if (vkAPI.CreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool.");
        }
    }

    return m_commandPool;
}

VkDescriptorPool VulkanDevice::getVkDescriptorPool()
{
    if (m_descriptorPool == VK_NULL_HANDLE)
    {
        const uint32_t maxSets = 30; // TODO: set correct max value.
        const uint64_t descriptorPoolCount = 7;
        const uint64_t maxDescriptorSetSize = descriptorPoolCount;
        std::array<VkDescriptorPoolSize, descriptorPoolCount> poolSizes;
        VkDescriptorPoolCreateInfo poolCreateInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                                                   .pNext = nullptr,
                                                   .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                                                   .maxSets = maxSets,
                                                   .poolSizeCount = descriptorPoolCount,
                                                   .pPoolSizes = poolSizes.data() };

        auto& vulkanPhysicalDevice = downcast(m_physicalDevice);
        const VulkanPhysicalDeviceInfo& physicalDeviceInfo = vulkanPhysicalDevice.getVulkanPhysicalDeviceInfo();
        const VkPhysicalDeviceLimits& devicePropertyLimists = physicalDeviceInfo.physicalDeviceProperties.limits;

        // TODO: set correct max value.
        uint32_t poolSize = devicePropertyLimists.maxBoundDescriptorSets;
        poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, poolSize };
        poolSizes[1] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, poolSize };
        poolSizes[2] = { VK_DESCRIPTOR_TYPE_SAMPLER, poolSize };
        poolSizes[3] = { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, poolSize };
        poolSizes[4] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, poolSize };
        poolSizes[5] = { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, poolSize };
        poolSizes[6] = { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, poolSize };

        VkResult result = vkAPI.CreateDescriptorPool(m_device, &poolCreateInfo, nullptr, &m_descriptorPool);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor pool.");
        }
    }

    return m_descriptorPool;
}

void VulkanDevice::createDevice(const std::unordered_set<uint32_t>& queueFamilyIndices)
{
    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;

    float queuePriority = 1.0f;
    for (const uint32_t queueFamilyIndex : queueFamilyIndices)
    {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
        deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
    }

    auto& vulkanPhysicalDevice = downcast(m_physicalDevice);

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &vulkanPhysicalDevice.getVulkanPhysicalDeviceInfo().physicalDeviceFeatures;

    std::vector<const char*> requiredDeviceExtensions = getRequiredDeviceExtensions();

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

    // TODO: Layer.

    VkPhysicalDevice physicalDevice = downcast(m_physicalDevice).getVkPhysicalDevice();
    VkResult result = vkAPI.CreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &m_device);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("failed to create logical device. {}", static_cast<uint32_t>(result)));
    }
}

const std::vector<const char*> VulkanDevice::getRequiredDeviceExtensions()
{
    std::vector<const char*> requiredDeviceExtensions;

    requiredDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    // TODO: check extension supported.
    auto& vulkanPhysicalDevice = downcast(m_physicalDevice);
    if (vulkanPhysicalDevice.getVulkanPhysicalDeviceInfo().portabilitySubset)
    {
        // TODO: define "VK_KHR_portability_subset"
        requiredDeviceExtensions.push_back("VK_KHR_portability_subset");
    }

    spdlog::info("Required Device extensions :");
    for (const auto& extension : requiredDeviceExtensions)
    {
        spdlog::info("{}{}", '\t', extension);
    }
    return requiredDeviceExtensions;
};

} // namespace jipu
