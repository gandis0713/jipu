#include "tflite_inference.h"

#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>

#include "tflite/c/c_api.h"
#include "tflite/core/c/c_api.h"

namespace jipu
{

TFLiteInference::TFLiteInference()
    : m_interpreter(nullptr)
    , m_model(nullptr)
    , m_inputImage(nullptr)
{
}

TFLiteInference::~TFLiteInference()
{
    if (m_interpreter)
    {
        TfLiteInterpreterDelete(m_interpreter);
    }
    if (m_model)
    {
        TfLiteModelDelete(m_model);
    }
}

bool TFLiteInference::setInputImage(std::unique_ptr<Image> image)
{
    if (!image)
    {
        spdlog::error("Input image is null");
        return false;
    }

    m_inputImage = std::move(image);
    return true;
}

bool TFLiteInference::loadModel(const std::vector<char>& modelBuffer)
{
    m_model = TfLiteModelCreate(modelBuffer.data(), modelBuffer.size());
    if (!m_model)
    {
        spdlog::error("Failed to load model");
        return false;
    }

    // 인터프리터 옵션 설정
    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    TfLiteInterpreterOptionsSetNumThreads(options, 4);

    // 인터프리터 생성
    m_interpreter = TfLiteInterpreterCreate(m_model, options);
    TfLiteInterpreterOptionsDelete(options);

    if (!m_interpreter)
    {
        spdlog::error("Failed to create interpreter");
        return false;
    }

    // 텐서 할당
    if (TfLiteInterpreterAllocateTensors(m_interpreter) != kTfLiteOk)
    {
        spdlog::error("Failed to allocate tensors");
        return false;
    }

    spdlog::info("Model loaded successfully");
    return true;
}

void TFLiteInference::preprocessImage(std::vector<float>& preprocessed)
{
    TfLiteTensor* inputTensor = TfLiteInterpreterGetInputTensor(m_interpreter, 0);
    if (!inputTensor)
        return;

    int expectedSize = 1;
    auto numberOfDims = TfLiteTensorNumDims(inputTensor);
    for (int i = 0; i < numberOfDims; i++)
    {
        expectedSize *= TfLiteTensorDim(inputTensor, i);
    }

    int32_t batchSize = TfLiteTensorDim(inputTensor, 0);
    int32_t width = TfLiteTensorDim(inputTensor, 1);
    int32_t height = TfLiteTensorDim(inputTensor, 2);
    int32_t channels = TfLiteTensorDim(inputTensor, 3);
    auto byteSize = TfLiteTensorByteSize(inputTensor);

    spdlog::info("Expected input shape: [{}, {}, {}, {}], byte size:[{}], expectedSize:[{}]",
                 batchSize, height, width, channels, byteSize, expectedSize);

    preprocessed.clear();
    preprocessed.resize(expectedSize);

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

std::vector<uint8_t> TFLiteInference::postprocessOutput(const float* output,
                                                        int outputSize)
{
    std::vector<uint8_t> result(outputSize);

    // 세그멘테이션 결과를 시각화 가능한 형태로 변환
    for (int i = 0; i < outputSize; i++)
    {
        // argmax를 통해 클래스 ID 추출
        int classId = static_cast<int>(output[i]);

        // 클래스별 색상 매핑 (간단한 예시)
        result[i] = static_cast<uint8_t>(classId * 15 % 255);
    }

    return result;
}

std::vector<uint8_t> TFLiteInference::runInference()
{
    if (!m_interpreter)
    {
        spdlog::error("Interpreter not initialized");
        return {};
    }

    // 입력 텐서 가져오기
    TfLiteTensor* inputTensor = TfLiteInterpreterGetInputTensor(m_interpreter, 0);
    if (!inputTensor)
    {
        spdlog::error("Failed to get input tensor");
        return {};
    }

    auto inputImage = static_cast<uint8_t*>(m_inputImage->getPixels());
    if (!inputImage)
    {
        spdlog::error("Input image data is empty");
        return {};
    }

    // 이미지 전처리
    std::vector<float> preprocessed;
    preprocessImage(preprocessed);

    // 입력 데이터 복사
    auto result = TfLiteTensorCopyFromBuffer(inputTensor, preprocessed.data(),
                                             preprocessed.size() * sizeof(float));
    if (result != kTfLiteOk)
    {
        spdlog::error("Failed to copy input data");
        return {};
    }

    // 추론 실행
    if (TfLiteInterpreterInvoke(m_interpreter) != kTfLiteOk)
    {
        spdlog::error("Failed to invoke interpreter");
        return {};
    }

    // 출력 텐서 가져오기
    const TfLiteTensor* outputTensor = TfLiteInterpreterGetOutputTensor(m_interpreter, 0);
    if (!outputTensor)
    {
        spdlog::error("Failed to get output tensor");
        return {};
    }

    // 출력 데이터 처리
    const float* outputData = static_cast<const float*>(TfLiteTensorData(outputTensor));
    int outputSize = TfLiteTensorByteSize(outputTensor) / sizeof(float);

    return postprocessOutput(outputData, outputSize);
}

} // namespace jipu