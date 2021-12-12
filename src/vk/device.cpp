#include "vk/device.h"

#include "vk/instance.h"
#include "vk/physical_device.h"
#include "vk/device_features.h"
#include "vk/allocation_callbacks.h"

namespace vks
{

Device::Device(PhysicalDevice* physical_device, 
              const QueueSettings& queue_settings, 
              const char** layers, 
              const char** device_extensions, 
              const DeviceFeatures* deviceFeatures, 
              AllocationCallbacks* allocator)
{
  
}

} // namespace vks

