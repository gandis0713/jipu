#pragma once

namespace jipu
{

class VulkanDevice;
class VulkanInflightResources final
{

public:
    VulkanInflightResources() = delete;
    explicit VulkanInflightResources(VulkanDevice* device);
    ~VulkanInflightResources();

private:
    [[maybe_unused]] VulkanDevice* m_device = nullptr;

private:
};

} // namespace jipu