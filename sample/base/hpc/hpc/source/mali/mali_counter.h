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
    { Counter::GeometryTotalCullPrimitives, { MaliGeomTotalCullPrim } },
    { Counter::GeometryVisiblePrimitives, { MaliGeomVisiblePrim } },
    { Counter::GeometrySampleCulledPrimitives, { MaliGeomSampleCullPrim } },
    { Counter::GeometryFaceXYPlaneCulledPrimitives, { MaliGeomFaceXYPlaneCullPrim } },
    { Counter::GeometryZPlaneCulledPrimitives, { MaliGeomZPlaneCullPrim } },
    { Counter::GeometryVisibleRate, { MaliGeomVisibleRate } },
    { Counter::GeometrySampleCulledRate, { MaliGeomSampleCullRate } },
    { Counter::GeometryFaceXYPlaneCulledRate, { MaliGeomFaceXYPlaneCullRate } },
    { Counter::GeometryZPlaneCulledRate, { MaliGeomZPlaneCullRate } },
};

} // namespace mali
} // namespace hpc