#pragma once

#include <chrono>
#include <functional>
#include <vector>

#include "hpc/counter.h"
#include "hpc/sampler.h"

namespace jipu
{

class HPCWatcher
{

public:
    using Listner = std::function<void(std::vector<hpc::Sample>)>;
    HPCWatcher(hpc::Sampler::Ptr sampler, Listner listner);

    void start();
    void stop();
    void update();

private:
    hpc::Sampler::Ptr m_sampler{ nullptr };
    Listner m_listner{};

private:
    uint32_t period = 1000; // 1 second
    std::chrono::milliseconds m_time = std::chrono::milliseconds::zero();

public:
    using Ptr = std::unique_ptr<HPCWatcher>;
};

} // namespace jipu