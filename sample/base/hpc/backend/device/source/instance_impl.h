#pragma once

#include "device/handle.h"
#include "device/instance.h"

namespace hpc
{
namespace device
{

class InstanceImpl : public Instance
{
public:
    explicit InstanceImpl(Handle& handle);
    ~InstanceImpl() override = default;

private:
    void init();

private:
    Handle& m_handle;
};

} // namespace device
} // namespace hpc