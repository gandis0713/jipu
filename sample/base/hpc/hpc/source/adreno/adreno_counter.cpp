#include "adreno_counter.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace adreno
{

// TODO
hpc::backend::Counter convertCounter(hpc::Counter counter)
{
    auto it = counterDependencies.find(counter);
    if (it != counterDependencies.end())
    {
        const auto& dependencies = counterDependencies.at(counter);

        if (dependencies.size() == 1)
        {
            return static_cast<hpc::backend::Counter>(dependencies[0]);
        }
    }

    spdlog::error("currently, not supported in adreno {}", static_cast<uint32_t>(counter));
    return 0;
}

// const std::unordered_map<Counter, std::vector<hwcpipe_counter>> counterDependencies{
//     { Counter::NonFragmentUtilization, { AdrenoFragQueueUtil } },
//     { Counter::FragmentUtilization, { AdrenoNonFragQueueUtil } },
//     { Counter::TilerUtilization, { AdrenoTilerUtil } },
//     { Counter::ExternalReadBytes, { AdrenoExtBusRdBy } },
//     { Counter::ExternalWriteBytes, { AdrenoExtBusWrBy } },
//     { Counter::ExternalReadStallRate, { AdrenoExtBusRdStallRate } },
//     { Counter::ExternalWriteStallRate, { AdrenoExtBusWrStallRate } },
//     { Counter::ExternalReadLatency0, { AdrenoExtBusRdLat0 } },
//     { Counter::ExternalReadLatency1, { AdrenoExtBusRdLat128 } },
//     { Counter::ExternalReadLatency2, { AdrenoExtBusRdLat192 } },
//     { Counter::ExternalReadLatency3, { AdrenoExtBusRdLat256 } },
//     { Counter::ExternalReadLatency4, { AdrenoExtBusRdLat320 } },
//     { Counter::ExternalReadLatency5, { AdrenoExtBusRdLat384 } },
//     { Counter::GeometryTotalInputPrimitives, { AdrenoGeomTotalPrim } },
// };

} // namespace adreno
} // namespace hpc