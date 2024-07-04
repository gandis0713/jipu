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
    explicit InstanceImpl(std::unique_ptr<Handle> handle);
    ~InstanceImpl() override = default;

private:
    void init();

private:
    std::unique_ptr<Handle> m_handle = nullptr;
};

} // namespace backend
} // namespace hpc