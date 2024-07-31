#pragma once

#include "export.h"
#include "gpu.h"

#include <memory>

namespace hpc
{
namespace backend
{

enum class DeviceType
{
    Adreno,
    Mali
};

class HPC_BACKEND_VISIBILITY Instance
{
public:
    static std::unique_ptr<Instance> create(const DeviceType type);

    virtual ~Instance() = default;

    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;

    virtual std::vector<std::unique_ptr<hpc::backend::GPU>> gpus() = 0;

protected:
    Instance() = default;
};

} // namespace backend
} // namespace hpc