#pragma once

#include <cstdint>

namespace jipu
{

void yuv420toRgb(const uint8_t* yData, const uint8_t* uData, const uint8_t* vData,
                 int width, int height, int yRowStride, int uvRowStride, int uvPixelStride,
                 uint8_t* outRgb);

}