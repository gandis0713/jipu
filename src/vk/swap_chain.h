#pragma once

namespace vkt
{

struct SwapChainCreateInfo
{
};

class SwapChain
{
    explicit SwapChain(const SwapChainCreateInfo info);
    ~SwapChain();
};
} // namespace vkt