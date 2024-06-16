#pragma once

#include <chrono>
#include <functional>
#include <unordered_map>

#include "hpc/counter.h"
#include "hpc/sampler.h"

namespace jipu
{

using Values = std::vector<hpc::Sample>;
using Listner = std::function<void(Values)>;

struct HPCWatcherDescriptor
{
    std::unique_ptr<hpc::Sampler> sampler = nullptr;
    std::vector<hpc::Counter> counters{};
    Listner listner{};
};

class HPCWatcher
{

public:
    HPCWatcher(HPCWatcherDescriptor descriptor);

    void start();
    void stop();
    void update();

private:
    HPCWatcherDescriptor m_descriptor{};

private:
    uint32_t period = 1000; // 1 second
    std::chrono::milliseconds m_time = std::chrono::milliseconds::zero();

public:
    using Ptr = std::unique_ptr<HPCWatcher>;
};

} // namespace jipu