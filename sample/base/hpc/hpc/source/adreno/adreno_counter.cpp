#include "adreno_counter.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace adreno
{

std::unordered_set<hpc::backend::Counter> convertCounter(const hpc::Counter counter)
{

    auto it = counterDependencies.find(counter);
    if (it == counterDependencies.end())
    {
        spdlog::error("currently, not supported in adreno {}", static_cast<uint32_t>(counter));
        return {};
    }

    std::unordered_set<hpc::backend::Counter> counters{};
    const auto& dependencies = counterDependencies.at(counter);
    for (const auto& dependency : dependencies)
    {
        counters.insert(static_cast<hpc::backend::Counter>(dependency));
    }

    return counters;
}

} // namespace adreno
} // namespace hpc