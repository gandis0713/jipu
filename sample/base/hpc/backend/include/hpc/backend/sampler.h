#pragma once

#include "export.h"

#include <system_error>
#include <unordered_map>
#include <vector>

namespace hpc
{
namespace backend
{

using Counter = uint32_t;

struct SamplerDescriptor
{
    std::vector<Counter> counters{};
};

class HPC_BACKEND_VISIBILITY Sampler
{
public:
    virtual ~Sampler() = default;

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    virtual std::error_code start() = 0;
    virtual std::error_code stop() = 0;

    virtual std::unordered_map<Counter, uint64_t> sample(std::vector<Counter> counter) = 0;

protected:
    Sampler() = default;
};

} // namespace backend
} // namespace hpc