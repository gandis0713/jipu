#pragma once

#include "export.h"
#include "hpc/gpu.h"

#include <memory>
#include <vector>

namespace hpc
{

enum class GPUType
{
    Mali,
    Adreno
};

struct InstanceDescriptor
{
    GPUType gpuType;
};

class HPC_EXPORT Instance
{
public:
    virtual ~Instance() = default;

public:
    static std::unique_ptr<Instance> create(const InstanceDescriptor& descriptor);

public:
    virtual std::vector<std::unique_ptr<hpc::GPU>> gpus() = 0;
};

} // namespace hpc