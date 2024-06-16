#pragma once

#include <memory>

#include "counter.h"
#include "export.h"
#include "sampler.h"

namespace hpc
{

class HPC_EXPORT GPU
{
public:
    virtual ~GPU() = default;

protected:
    GPU() = default;

public:
    virtual std::unique_ptr<Sampler> create(const SamplerDescriptor& descriptor) = 0;
    virtual const std::vector<Counter> counters() const = 0;

public:
    using Ptr = std::unique_ptr<GPU>;
};

} // namespace hpc