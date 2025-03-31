#pragma once

#include "june/june.h"
#include "june_vulkan_service.h"

namespace jipu
{

class JuneVulkanService1 : public JuneVulkanService
{
public:
    JuneVulkanService1(const JuneServiceDescriptor& descriptor);
    ~JuneVulkanService1();

    void begin() override;
    void work() override;

private:
    // JuneSharedMemory m_juneSharedMemory{ nullptr };
    // JuneApiMemory m_juneApiMemory{ nullptr };
};

} // namespace jipu