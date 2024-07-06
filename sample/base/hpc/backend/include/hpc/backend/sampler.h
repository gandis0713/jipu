#pragma once

#include "export.h"

#include <system_error>

namespace hpc
{
namespace backend
{

class HPC_BACKEND_VISIBILITY Sampler
{
public:
    virtual ~Sampler() = default;

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    virtual std::error_code start() = 0;
    virtual std::error_code stop() = 0;
    virtual void setCounters(const std::vector<uint32_t>& counters) = 0;

protected:
    Sampler() = default;
};

} // namespace backend
} // namespace hpc