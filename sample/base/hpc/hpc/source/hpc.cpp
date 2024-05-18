#include "hpc/hpc.h"

#include "mali/mali_hpc.h"

namespace hpc
{

std::vector<GPU::Ptr> gpus()
{
    return mali::gpus();
}

} // namespace hpc