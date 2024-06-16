#pragma once

#include "export.h"

#include <vector>

namespace hpc
{

enum class GPUVendor
{
    Mali,
    Adreno
};

class GPU;
class HPC_EXPORT Instance
{
public:
    virtual ~Instance() = default;

public:
    static std::unique_ptr<Instance> create(const GPUVendor vendor);

public:
    virtual std::vector<std::unique_ptr<GPU>> gpus() = 0;
};

} // namespace hpc