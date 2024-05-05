#pragma once

#include <cstdint>

namespace hpc
{

enum class Counter : uint16_t
{
    NonFragmentUtilization = 0,
    FragmentUtilization,
    TilerUtilization,
    ExternalReadBytes,
    ExternalWriteBytes,
    ExternalReadStallRate,
    ExternalWriteStallRate,
    ExternalReadLatency0,
    ExternalReadLatency1,
    ExternalReadLatency2,
    ExternalReadLatency3,
    ExternalReadLatency4,
    ExternalReadLatency5,
    GeometryTotalInputPrimitives,
    GeometryTotalCullPrimitives,
    GeometryVisiblePrimitives,
    GeometryFaceXYPlaneCulledPrimitives,
    GeometryZPlaneCulledPrimitives,
    GeometryVisibleRate,
    GeometryFaceXYPlaneCulledRate,
    GeometryZPlaneCulledRate,
    Count // total enum count
};

} // namespace hpc