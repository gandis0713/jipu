#pragma once

#include "jipu/common/dylib.h"
#include "june/june.h"
#include "june_api.h"
#include "june_service.h"

#include <filesystem>
#include <unordered_set>

#include "jipu/native/adapter.h"
#include "jipu/native/buffer.h"
#include "jipu/native/command_buffer.h"
#include "jipu/native/command_encoder.h"
#include "jipu/native/device.h"
#include "jipu/native/instance.h"
#include "jipu/native/physical_device.h"
#include "jipu/native/pipeline.h"
#include "jipu/native/pipeline_layout.h"
#include "jipu/native/queue.h"
#include "jipu/native/surface.h"
#include "jipu/native/swapchain.h"

#include "jipu/native/vulkan/vulkan_adapter.h"
#include "jipu/native/vulkan/vulkan_command.h"
#include "jipu/native/vulkan/vulkan_command_buffer.h"
#include "jipu/native/vulkan/vulkan_device.h"
#include "jipu/native/vulkan/vulkan_queue.h"
#include "jipu/native/vulkan/vulkan_surface.h"

namespace jipu
{

class JuneVulkanService : public JuneService
{

public:
    JuneVulkanService(const JuneServiceDescriptor& descriptor);
    ~JuneVulkanService();

protected:
    void begin() override;
    void beforeWork() override;
    void end() override;

private:
    void createInstance();
    void createAdapter();
    void getPhysicalDevices();
    void createSurface();
    void createDevice();
    void createSwapchain();
    void createQueue();

protected:
    std::unique_ptr<Instance> m_instance = nullptr;
    std::unique_ptr<Adapter> m_adapter = nullptr;
    std::vector<std::unique_ptr<PhysicalDevice>> m_physicalDevices{};
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
    std::unique_ptr<CommandEncoder> m_commandEncoder = nullptr;
    TextureView* m_renderView = nullptr;

protected:
    VkInstance m_vkInstance = VK_NULL_HANDLE;
    VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_vkDevice = VK_NULL_HANDLE;

protected:
    JuneInstance m_juneInstance{ nullptr };
    JuneApiContext m_juneApiContext{ nullptr };
};

} // namespace jipu