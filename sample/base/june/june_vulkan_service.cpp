#include "june_vulkan_service.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

JuneVulkanService::JuneVulkanService(const JuneServiceDescriptor& descriptor)
    : JuneService(descriptor)
{
}

JuneVulkanService::~JuneVulkanService()
{
}

void JuneVulkanService::begin()
{
    // create vulkan instance by native
    {
        createInstance();
        createAdapter();
        getPhysicalDevices();
        createSurface();
        createDevice();
        createQueue();
        createSwapchain();
    }

    // Create Api Context
    {
        auto vulkanAdapter = static_cast<VulkanAdapter*>(m_adapter.get());
        m_vkInstance = vulkanAdapter->getVkInstance();
        m_vkPhysicalDevice = vulkanAdapter->getVkPhysicalDevice(0);
        m_vkDevice = static_cast<VulkanDevice*>(m_device.get())->getVkDevice();

        JuneVulkanContextDescriptor juneVulkanContextDescriptor{};
        juneVulkanContextDescriptor.chain.sType = JuneSType_VulkanContext;
        juneVulkanContextDescriptor.vkInstance = m_vkInstance;
        juneVulkanContextDescriptor.vkPhysicalDevice = m_vkPhysicalDevice;
        juneVulkanContextDescriptor.vkDevice = m_vkDevice;

        JuneApiContextDescriptor juneApiContextDescriptor{
            .nextInChain = &juneVulkanContextDescriptor.chain
        };
        m_juneApiContext = m_juneAPI.InstanceCreateApiContext(m_juneInstance, &juneApiContextDescriptor);
    }
}

void JuneVulkanService::beforeWork()
{
}

void JuneVulkanService::end()
{
    // end vulkan instance
    {
        m_swapchain.reset();
        m_queue.reset();
        m_surface.reset();
        m_device.reset();
        m_physicalDevices.clear();
        m_adapter.reset();
        m_instance.reset();
    }
}

void JuneVulkanService::createInstance()
{
    InstanceDescriptor descriptor;
    m_instance = Instance::create(descriptor);
}

void JuneVulkanService::createAdapter()
{
    AdapterDescriptor descriptor;
    descriptor.type = BackendAPI::kVulkan;
    m_adapter = m_instance->createAdapter(descriptor);
}

void JuneVulkanService::getPhysicalDevices()
{
    m_physicalDevices = m_adapter->getPhysicalDevices();
}

void JuneVulkanService::createSurface()
{
    if (!m_descriptor.windowHandle)
    {
        spdlog::debug("Window handle is null.");
        return;
    }

    SurfaceDescriptor descriptor;
    descriptor.windowHandle = m_descriptor.windowHandle;
    m_surface = m_adapter->createSurface(descriptor);
}

void JuneVulkanService::createSwapchain()
{
    if (!m_surface)
    {
        spdlog::debug("Surface is null.");
        return;
    }

#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat textureFormat = TextureFormat::kRGBA8UnormSrgb;
#else
    TextureFormat textureFormat = TextureFormat::kBGRA8UnormSrgb;
#endif
    SwapchainDescriptor descriptor{
        .surface = m_surface.get(),
        .textureFormat = textureFormat,
        .presentMode = PresentMode::kFifo,
        .colorSpace = ColorSpace::kSRGBNonLinear,
        .width = m_descriptor.width,
        .height = m_descriptor.height,
        .queue = m_queue.get()
    };

    m_swapchain = m_device->createSwapchain(descriptor);
}

void JuneVulkanService::createDevice()
{
    // TODO: select suit device.
    PhysicalDevice* physicalDevice = m_physicalDevices[0].get();

    DeviceDescriptor descriptor;
    m_device = physicalDevice->createDevice(descriptor);
}

void JuneVulkanService::createQueue()
{
    QueueDescriptor descriptor{};

    m_queue = m_device->createQueue(descriptor);
}

void JuneVulkanService::createApiContext(const std::string& label)
{
    auto vulkanAdapter = static_cast<VulkanAdapter*>(m_adapter.get());
    m_vkInstance = vulkanAdapter->getVkInstance();
    m_vkPhysicalDevice = vulkanAdapter->getVkPhysicalDevice(0);
    m_vkDevice = static_cast<VulkanDevice*>(m_device.get())->getVkDevice();

    JuneVulkanContextDescriptor juneVulkanContextDescriptor{};
    juneVulkanContextDescriptor.chain.sType = JuneSType_VulkanContext;
    juneVulkanContextDescriptor.vkInstance = m_vkInstance;
    juneVulkanContextDescriptor.vkPhysicalDevice = m_vkPhysicalDevice;
    juneVulkanContextDescriptor.vkDevice = m_vkDevice;

    JuneApiContextDescriptor juneApiContextDescriptor;
    juneApiContextDescriptor.nextInChain = &juneVulkanContextDescriptor.chain;
    juneApiContextDescriptor.label.data = label.data();
    juneApiContextDescriptor.label.length = label.length();

    m_juneApiContext = m_juneAPI.InstanceCreateApiContext(m_juneInstance, &juneApiContextDescriptor);
}

} // namespace jipu
