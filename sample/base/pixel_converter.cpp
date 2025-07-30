#include "pixel_converter.h"

#include <algorithm>

namespace jipu
{

void yuv420toRgb(const uint8_t* yData, const uint8_t* uData, const uint8_t* vData,
                 int width, int height, int yRowStride, int uvRowStride, int uvPixelStride,
                 uint8_t* outRgb)
{

    for (int y = 0; y < height; ++y)
    {
        const uint8_t* yRow = yData + y * yRowStride;
        // U, V 데이터는 Y에 비해 1/2 크기를 가집니다.
        const uint8_t* uRow = uData + (y / 2) * uvRowStride;
        const uint8_t* vRow = vData + (y / 2) * uvRowStride;

        for (int x = 0; x < width; ++x)
        {
            // Y 값
            int32_t yValue = yRow[x];

            // U, V 값 (uvPixelStride를 고려하여 인덱스 계산)
            int32_t uValue = uRow[(x / 2) * uvPixelStride] - 128;
            int32_t vValue = vRow[(x / 2) * uvPixelStride] - 128;

            // YUV to RGB 변환 공식 (부동 소수점 연산을 피하기 위해 정수 연산 사용)
            int32_t r = yValue + (1.370705 * vValue);
            int32_t g = yValue - (0.698001 * vValue) - (0.337633 * uValue);
            int32_t b = yValue + (1.732446 * uValue);

            // 0-255 범위로 클리핑
            r = std::max(0, std::min(255, r));
            g = std::max(0, std::min(255, g));
            b = std::max(0, std::min(255, b));

            // 결과 버퍼에 저장
            int rgbIndex = (y * width + x) * 3;
            outRgb[rgbIndex] = r;
            outRgb[rgbIndex + 1] = g;
            outRgb[rgbIndex + 2] = b;
        }
    }
}

} // namespace jipu
