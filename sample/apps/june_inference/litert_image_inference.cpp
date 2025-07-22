#include "litert_image_inference.h"

#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>

namespace jipu
{

LiteRtImageInference::LiteRtImageInference()
    : m_model(nullptr)
    , m_inputImage(nullptr)
{
}

LiteRtImageInference::~LiteRtImageInference()
{
}

bool LiteRtImageInference::setInputImage(Image* image)
{
    if (!image)
    {
        spdlog::error("Input image is null");
        return false;
    }

    m_inputImage = image;
    return true;
}

bool LiteRtImageInference::loadModel(const std::vector<char>& modelBuffer)
{
    auto status = LiteRtCreateModelFromBuffer(modelBuffer.data(), modelBuffer.size(), m_model);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create model from buffer");
        return false;
    }

    spdlog::info("Model loaded successfully");
    return true;
}

int32_t LiteRtImageInference::getBatchSize()
{
    return 0;
}

int32_t LiteRtImageInference::getWidth()
{
    return 0;
}

int32_t LiteRtImageInference::getHeight()
{
    return 0;
}

int32_t LiteRtImageInference::getInputChannel()
{
    return 0;
}

int32_t LiteRtImageInference::getOutputChannel()
{
    return 0;
}

size_t LiteRtImageInference::getInputByteSize()
{
    return 0;
}

size_t LiteRtImageInference::getInputSize()
{
    return 0;
}

size_t LiteRtImageInference::getOutputByteSize()
{
    return 0;
}

size_t LiteRtImageInference::getOutputSize()
{
    return 0;
}

void LiteRtImageInference::preprocessImage(std::vector<float>& preprocessed)
{
    size_t inputSize = getInputSize();
    int32_t batchSize = getBatchSize();
    int32_t width = getWidth();
    int32_t height = getHeight();
    int32_t channels = getInputChannel();
    size_t inputByteSize = getInputByteSize();

    spdlog::info("Expected input shape: [{}, {}, {}, {}], byte size:[{}], inputSize:[{}]",
                 batchSize, height, width, channels, inputByteSize, inputSize);

    preprocessed.clear();
    preprocessed.resize(inputSize);

    auto inputImage = static_cast<uint8_t*>(m_inputImage->getPixels());
    int imgWidth = m_inputImage->getWidth();
    int imgHeight = m_inputImage->getHeight();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // 이미지 리사이징 및 정규화
            int srcX = x * imgWidth / width;
            int srcY = y * imgHeight / height;

            for (int c = 0; c < channels; c++)
            {
                int srcIdx = (srcY * imgWidth + srcX) * channels + c;
                int dstIdx = (y * width + x) * channels + c;

                // 정규화 (0-255 -> 0-1 또는 -1~1)
                preprocessed[dstIdx] = inputImage[srcIdx] / 255.0f;
            }
        }
    }

    spdlog::info("Preprocessed data size: {}", preprocessed.size());
}

std::vector<uint8_t> LiteRtImageInference::postprocessOutput(const float* output,
                                                             int outputSize)
{
    std::vector<uint8_t> result(outputSize);

    // 세그멘테이션 결과를 시각화 가능한 형태로 변환
    for (int i = 0; i < outputSize; i++)
    {
        result[i] = static_cast<uint8_t>(output[i] * 255);
    }

    return result;
}

std::vector<uint8_t> LiteRtImageInference::runInference()
{
    return {};
}

} // namespace jipu