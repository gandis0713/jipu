#pragma once

#include <memory>

#include "counter.h"
#include "export.h"
#include "sampler.h"

namespace jipu
{
namespace hpc
{

class HPC_EXPORT GPU
{
public:
    virtual ~GPU() = default;

protected:
    GPU() = default;

public:
    virtual Sampler::Ptr create(SamplerDescriptor descriptor) = 0;

public:
    const std::vector<Counter>& counters() const;

protected:
    std::vector<Counter> m_counters{};

public:
    using Ptr = std::unique_ptr<GPU>;
};

} // namespace hpc
} // namespace jipu