#include "gpu/swap_chain.h"

#include "gpu/device.h"
#include "gpu/surface.h"

namespace vkt
{

SwapChain::SwapChain(Device* device, SwapChainCreateInfo info) noexcept
    : m_device(device)
    , m_surface(std::move(info.surface))
{
}

} // namespace vkt
