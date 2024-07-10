#pragma once

/// Adreno GPU series.
enum class AdrenoSeries : uint32_t
{
    HPC_GPU_ADRENO_SERIES_UNKNOWN = 0,
    /// Adreno A6XX GPUs ((>= 600 && < 700) || == 702)
    HPC_GPU_ADRENO_SERIES_A6XX,
    /// Adreno A5XX GPUs (>= 500 && < 600)
    HPC_GPU_ADRENO_SERIES_A5XX,
};
