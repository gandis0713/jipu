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

    JuneServiceShareObjects getSharingObject() const override;
    void setSharedObjects(const JuneServiceShareObjects& sharedObjects) override;

private:
    JuneServiceShareObjects m_sharingObjects{};
    JuneServiceShareObjects m_sharedObjects{};
};

} // namespace jipu