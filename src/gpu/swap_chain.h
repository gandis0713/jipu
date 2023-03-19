#pragma once

#include <memory>

namespace vkt
{

class Device;
class Surface;

struct SwapChainCreateInfo
{
    std::unique_ptr<Surface> surface{ nullptr };
};

class SwapChain
{
public:
    SwapChain() = delete;
    SwapChain(Device* device, SwapChainCreateInfo info) noexcept;
    virtual ~SwapChain() noexcept = default;

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

protected:
    Device* m_device;
    std::unique_ptr<Surface> m_surface;
};

} // namespace vkt
