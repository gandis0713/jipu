#pragma once

#include "export.h"

#include <system_error>
#include <unordered_map>
#include <unordered_set>

namespace hpc
{
namespace backend
{

using Counter = uint32_t;
using Sample = uint64_t;

struct SamplerDescriptor
{
    std::unordered_set<Counter> counters{};
};

class HPC_BACKEND_VISIBILITY Sampler
{
public:
    virtual ~Sampler() = default;

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    virtual std::error_code start() = 0;
    virtual std::error_code stop() = 0;

    virtual std::unordered_map<Counter, Sample> sample(std::unordered_set<Counter> counter) = 0;

protected:
    Sampler() = default;
};

} // namespace backend
} // namespace hpc