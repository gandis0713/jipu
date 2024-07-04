#pragma once

#include "hpc/backend/handle.h"
#include "hpc/backend/instance.h"

namespace hpc
{
namespace backend
{

class InstanceImpl : public Instance
{
public:
    static std::unique_ptr<Instance> create(const BackendType type);

protected:
    explicit InstanceImpl() = default;
    ~InstanceImpl() override = default;
};

} // namespace backend
} // namespace hpc