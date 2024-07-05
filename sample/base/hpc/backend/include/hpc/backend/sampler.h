#pragma once

#include "export.h"

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

protected:
    Sampler() = default;
};

} // namespace backend
} // namespace hpc