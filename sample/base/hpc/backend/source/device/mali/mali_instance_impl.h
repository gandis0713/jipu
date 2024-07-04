#pragma once

#include "hpc/backend/instance.h"

namespace hpc
{
namespace backend
{

class Handle;
class MaliInstanceImpl : public Instance
{
public:
    static std::unique_ptr<Instance> create();
    explicit MaliInstanceImpl(std::unique_ptr<Handle> handle);
    ~MaliInstanceImpl() override = default;

private:
    std::unique_ptr<Handle> m_handle = nullptr;
};

} // namespace backend
} // namespace hpc