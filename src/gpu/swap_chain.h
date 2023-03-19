#pragma once

namespace vkt
{

struct SwapChainCreateInfo
{
};

class SwapChain
{
public:
    SwapChain(SwapChainCreateInfo info) noexcept {};
    virtual ~SwapChain() noexcept = default;

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;
};

} // namespace vkt
