#pragma once

#include "export.h"
#include "sampler.h"

#include <memory>

namespace hpc
{
namespace backend
{

class Instance;
class HPC_BACKEND_VISIBILITY GPU
{
public:
    virtual ~GPU() = default;

    virtual Instance& getInstance() = 0;
    virtual std::unique_ptr<Sampler> createSampler() = 0;

protected:
    GPU() = default;
};

} // namespace backend
} // namespace hpc