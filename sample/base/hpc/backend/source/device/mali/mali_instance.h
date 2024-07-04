#pragma once

#include "common/cast.h"
#include "hpc/backend/handle.h"
#include "hpc/backend/instance.h"

namespace hpc
{
namespace backend
{
namespace mali
{

class MaliInstance : public Instance
{
public:
    static std::unique_ptr<Instance> create();
    explicit MaliInstance(std::unique_ptr<Handle> handle);
    ~MaliInstance() override = default;

private:
    std::unique_ptr<Handle> m_handle = nullptr;
};
DOWN_CAST(MaliInstance, Instance)

} // namespace mali
} // namespace backend
} // namespace hpc