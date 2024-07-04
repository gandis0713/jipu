#pragma once

#include "instance_impl.h"

namespace hpc
{
namespace backend
{

class Handle;
class AdrenoInstanceImpl : public InstanceImpl
{
public:
    static std::unique_ptr<Instance> create();

    explicit AdrenoInstanceImpl(std::unique_ptr<Handle> handle);
    ~AdrenoInstanceImpl() override = default;

private:
    void init();

private:
    std::unique_ptr<Handle> m_handle = nullptr;
};

} // namespace backend
} // namespace hpc