#include "hpc.h"

#include "mali/mali_gpus.h"

namespace jipu
{
namespace hpc
{

std::vector<GPU::Ptr> gpus()
{
    return mali::gpus();
}

} // namespace hpc
} // namespace jipu