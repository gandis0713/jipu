#pragma once

#include <vulkan/vulkan.h>

#include <list>
#include <memory>
#include <vector>

namespace vkt {
class Instance;
class PhysicalDevice;
class AllocationCallbacks;
class DeviceFeatures;
class Queue;

struct QueueSetting {
  int queue_family_index = -1;
  std::vector<float> queue_piorities;
};

using QueueSettings = std::vector<QueueSetting>;

class Device {

public:
  Device(PhysicalDevice *physical_device, const QueueSettings &queue_settings,
         const char **layers, const char **device_extensions,
         const DeviceFeatures *deviceFeatures = nullptr,
         AllocationCallbacks *allocator = nullptr);
  Device(const Device &device) = delete;
  Device(Device &&device) = delete;

  Device &operator=(const Device &device) = delete;
  Device &operator=(Device &&device) = delete;

private:
  VkDevice device_;

  std::shared_ptr<Instance> instance_;
  std::shared_ptr<PhysicalDevice> physical_device_;
  std::shared_ptr<AllocationCallbacks> allocation_callbacks_;

  std::list<std::shared_ptr<Queue>> queue_list_;
};

} // namespace vkt