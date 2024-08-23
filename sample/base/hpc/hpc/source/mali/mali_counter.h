#pragma once

#include "hpc/counter.h"

#include <hwcpipe/hwcpipe_counter.h>
#include <unordered_map>
#include <vector>

namespace hpc
{
namespace mali
{

// extern const std::unordered_map<Counter, std::vector<hwcpipe_counter>> counterDependencies;
const std::unordered_map<Counter, std::vector<hwcpipe_counter>> counterDependencies{
    { Counter::NonFragmentUtilization, { MaliNonFragQueueUtil } },
    { Counter::FragmentUtilization, { MaliFragQueueUtil } },
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
    { Counter::GeometryTotalCullPrimitives, { MaliGeomTotalCullPrim } },
    { Counter::GeometryVisiblePrimitives, { MaliGeomVisiblePrim } },
    { Counter::GeometrySampleCulledPrimitives, { MaliGeomSampleCullPrim } },
    { Counter::GeometryFaceXYPlaneCulledPrimitives, { MaliGeomFaceXYPlaneCullPrim } },
    { Counter::GeometryZPlaneCulledPrimitives, { MaliGeomZPlaneCullPrim } },
    { Counter::GeometryVisibleRate, { MaliGeomVisibleRate } },
    { Counter::GeometrySampleCulledRate, { MaliGeomSampleCullRate } },
    { Counter::GeometryFaceXYPlaneCulledRate, { MaliGeomFaceXYPlaneCullRate } },
    { Counter::GeometryZPlaneCulledRate, { MaliGeomZPlaneCullRate } },
    { Counter::MMUL3Hit, { MaliMMUL3Hit } },
    { Counter::MMUL2Hit, { MaliMMUL2Hit } },
    { Counter::MMUS2L3Hit, { MaliMMUS2L3Hit } },
    { Counter::MMUS2L2Hit, { MaliMMUS2L2Hit } },
    { Counter::L2CacheL1Read, { MaliL2CacheL1Rd } },
    { Counter::L2CacheL1ReadStallRate, { MaliL2CacheL1RdStallCy } },
    { Counter::L2CacheL1Write, { MaliL2CacheL1Wr } },
    { Counter::L2CacheRead, { MaliL2CacheRd } },
    { Counter::L2CacheReadStallRate, { MaliL2CacheRdStallCy } },
    { Counter::L2CacheWrite, { MaliL2CacheWr } },
    { Counter::L2CacheWriteStallRate, { MaliL2CacheWrStallCy } },
    { Counter::L2ReadByte, { MaliSCBusLSL2RdBy } },
};

} // namespace mali
} // namespace hpc