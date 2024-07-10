#pragma once

#include "export.h"

#include <system_error>
#include <vector>

namespace hpc
{
namespace backend
{

struct SamplerDescriptor
{
    std::vector<uint32_t> counters{};
};

class HPC_BACKEND_VISIBILITY Sampler
{
public:
    virtual ~Sampler() = default;

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    virtual std::error_code start() = 0;
    virtual std::error_code stop() = 0;

protected:
    Sampler() = default;
};

} // namespace backend
} // namespace hpc