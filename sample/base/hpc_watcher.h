#pragma once

#include <chrono>

#include "hpc/sampler.h"

namespace jipu
{

class HPCWatcher
{
public:
    HPCWatcher(hpc::Sampler::Ptr sampler);

    void update();

private:
    hpc::Sampler::Ptr m_sampler{ nullptr };

private:
    uint32_t period = 1000; // 1 second
    std::chrono::milliseconds m_time = std::chrono::milliseconds::zero();

public:
    using Ptr = std::unique_ptr<HPCWatcher>;
};

} // namespace jipu