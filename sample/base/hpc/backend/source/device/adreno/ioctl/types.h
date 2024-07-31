#pragma once

/// Adreno GPU series.
enum class AdrenoSeries : uint32_t
{
    UNKNOWN = 0,
    /// Adreno A6XX GPUs ((>= 600 && < 700) || == 702)
    A6XX,
    /// Adreno A5XX GPUs (>= 500 && < 600)
    A5XX,
};
