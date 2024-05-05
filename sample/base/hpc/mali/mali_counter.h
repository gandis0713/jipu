#pragma once

#include "counter.h"

#include <hwcpipe/hwcpipe_counter.h>
#include <unordered_map>
#include <vector>

namespace jipu
{
namespace hpc
{
namespace mali
{

// extern const std::unordered_map<Counter, std::vector<hwcpipe_counter>> counterDependencies;
const std::unordered_map<Counter, std::vector<hwcpipe_counter>> counterDependencies{
    { Counter::NonFragmentUtilization, { MaliFragQueueUtil } },
    { Counter::FragmentUtilization, { MaliNonFragQueueUtil } },
    { Counter::TilerUtilization, { MaliTilerUtil } },
    { Counter::ExternalReadBytes, { MaliExtBusRdBy } },
    { Counter::ExternalWriteBytes, { MaliExtBusWrBy } },
    { Counter::ExternalReadStallRate, { MaliExtBusRdStallRate } },
    { Counter::ExternalWriteStallRate, { MaliExtBusWrStallRate } },
    { Counter::ExternalReadLatency0, { MaliExtBusRdLat0 } },
    { Counter::ExternalReadLatency1, { MaliExtBusRdLat128 } },
    { Counter::ExternalReadLatency2, { MaliExtBusRdLat192 } },
    { Counter::ExternalReadLatency3, { MaliExtBusRdLat256 } },
    { Counter::ExternalReadLatency4, { MaliExtBusRdLat320 } },
    { Counter::ExternalReadLatency5, { MaliExtBusRdLat384 } },
    { Counter::GeometryTotalInputPrimitives, { MaliGeomTotalPrim } },
};

} // namespace mali
} // namespace hpc
} // namespace jipu