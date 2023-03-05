#include "swap_chain.h"

namespace vkt
{

SwapChain::SwapChain(const SwapChainCreateInfo info) : m_device(info.device), m_surface(info.surface) {}

SwapChain::~SwapChain() {}

} // namespace vkt
