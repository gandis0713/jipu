#pragma once

#include "jipu/swapchain.h"

namespace jipu
{

class Device;
class OffscreenSwapchain final : public Swapchain
{
public:
    OffscreenSwapchain() = delete;
    explicit OffscreenSwapchain(Device* device, const SwapchainDescriptor& descriptor);
    ~OffscreenSwapchain() override = default;

    OffscreenSwapchain(const OffscreenSwapchain&) = delete;
    OffscreenSwapchain& operator=(const OffscreenSwapchain&) = delete;

    void present(Queue* queue) override;
    int acquireNextTexture() override;

private:
    int m_currentTextureIndex = -1;
};

} // namespace jipu