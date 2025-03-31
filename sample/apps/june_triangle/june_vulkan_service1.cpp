#include "june_vulkan_service1.h"

#include <spdlog/spdlog.h>

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

    // Create Shared Memory
    {
    }

    // Create Api Memory
    {
    }

    // Create Resource
    {
    }
}

void JuneVulkanService1::work()
{
}

} // namespace jipu
