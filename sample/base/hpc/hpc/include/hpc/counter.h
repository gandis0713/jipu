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
    GeometrySampleCulledPrimitives,
    GeometryFaceXYPlaneCulledPrimitives,
    GeometryZPlaneCulledPrimitives,
    GeometryVisibleRate,
    GeometrySampleCulledRate,
    GeometryFaceXYPlaneCulledRate,
    GeometryZPlaneCulledRate,
    MMUL3Hit,
    MMUL2Hit,
    MMUS2L3Hit,
    MMUS2L2Hit,
    L2CacheL1Read,
    L2CacheL1ReadStallRate,
    L2CacheL1Write,
    L2CacheRead,
    L2CacheReadStallRate,
    L2CacheWrite,
    L2CacheWriteStallRate,
    L2ReadByte,
    Count // total enum count
};

} // namespace hpc