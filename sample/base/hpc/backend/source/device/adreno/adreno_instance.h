#pragma once

#include "common/cast.h"
#include "handle.h"
#include "hpc/backend/instance.h"

namespace hpc
{
namespace backend
{
namespace adreno
{

class AdrenoInstance : public Instance
{
public:
    static std::unique_ptr<Instance> create();

    explicit AdrenoInstance(std::unique_ptr<Handle> handle);
    ~AdrenoInstance() override = default;

private:
    void init();

private:
    std::unique_ptr<Handle> m_handle = nullptr;
};
DOWN_CAST(AdrenoInstance, Instance)

} // namespace adreno
} // namespace backend
} // namespace hpc