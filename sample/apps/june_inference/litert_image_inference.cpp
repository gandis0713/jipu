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
    LiteRtStatus status = LiteRtCreateModelFromBuffer(modelBuffer.data(), modelBuffer.size(), &m_model);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create model from buffer");
        return false;
    }

    status = LiteRtCreateEnvironment(0, nullptr, &m_environment);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create environment");
        return false;
    }

    status = LiteRtCreateOptions(&m_options);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create options");
        return false;
    }

    // m_acceleratorType = kLiteRtHwAcceleratorGpu; // Default to GPU, can be changed later
    status = LiteRtSetOptionsHardwareAccelerators(m_options, m_acceleratorType);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to set hardware accelerators");
        return false;
    }

    status = LiteRtCreateCompiledModel(m_environment, m_model, m_options, &m_compiledModel);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create compiled model");
        return false;
    }

    LiteRtSubgraph subgraph;
    status = LiteRtGetModelSubgraph(m_model, 0, &subgraph);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get model subgraph");
        return false;
    }

    LiteRtParamIndex numInputs;
    status = LiteRtGetNumSubgraphInputs(subgraph, &numInputs);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get number of subgraph inputs");
        return false;
    }

    spdlog::info("Model loaded with {} inputs", numInputs);

    LiteRtLayout layout = { 1, false, { 3, 256, 256 }, { 0 } }; // Example layout for a 3-channel image of size 256x256
    LiteRtRankedTensorType kInput0TensorType{ .element_type = kLiteRtElementTypeFloat32,
                                              .layout = layout };

    std::vector<LiteRtTensorBuffer> inputTensorBuffers;
    inputTensorBuffers.reserve(numInputs);
    for (auto i = 0; i < numInputs; ++i)
    {
        LiteRtTensorBufferRequirements tensorBufferRequirements;
        status = LiteRtGetCompiledModelInputBufferRequirements(
            m_compiledModel, /*signature_index=*/0, i,
            &tensorBufferRequirements);

        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get input buffer requirements for index {}", i);
            return false;
        }

        LiteRtTensorBufferType tensorBufferType;
        status = LiteRtGetTensorBufferRequirementsSupportedTensorBufferType(
            tensorBufferRequirements, /*type_index=*/0, &tensorBufferType);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor buffer type");
            return false;
        }

        size_t tensorBufferSize;
        status = LiteRtGetTensorBufferRequirementsBufferSize(
            tensorBufferRequirements, &tensorBufferSize);

        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor buffer size");
            return false;
        }

        LiteRtTensorBuffer tensorBuffer;
        status = LiteRtCreateManagedTensorBuffer(
            m_environment, tensorBufferType, &kInput0TensorType,
            tensorBufferSize, &tensorBuffer);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to create managed tensor buffer for input index {}", i);
            return false;
        }

        inputTensorBuffers.push_back(tensorBuffer);
    }

    spdlog::info("Model loaded with {} input tensor buffers", inputTensorBuffers.size());

    LiteRtParamIndex numOutputs;
    status = LiteRtGetNumSubgraphOutputs(subgraph, &numOutputs);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get number of subgraph outputs");
        return false;
    }

    spdlog::info("Model loaded with {} output tensors", numOutputs);
    std::vector<LiteRtTensorBuffer> outputTensorBuffers;
    outputTensorBuffers.reserve(numOutputs);
    for (auto i = 0; i < numOutputs; ++i)
    {
        LiteRtTensorBufferRequirements tensorBufferRequirements;
        status = LiteRtGetCompiledModelOutputBufferRequirements(
            m_compiledModel, /*signature_index=*/0, i,
            &tensorBufferRequirements);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get output buffer requirements for index {}", i);
            return false;
        }

        LiteRtTensorBufferType tensorBufferType;
        status = LiteRtGetTensorBufferRequirementsSupportedTensorBufferType(
            tensorBufferRequirements, /*type_index=*/0, &tensorBufferType);
        size_t tensorBufferSize;
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor buffer type for output index {}", i);
            return false;
        }

        status = LiteRtGetTensorBufferRequirementsBufferSize(
            tensorBufferRequirements, &tensorBufferSize);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor buffer size for output index {}", i);
            return false;
        }

        spdlog::info("Creating output tensor buffer for index {} with size {}", i, tensorBufferSize);

        LiteRtTensorBuffer tensorBuffer;
        status = LiteRtCreateManagedTensorBuffer(
            m_environment, tensorBufferType, &kInput0TensorType,
            tensorBufferSize, &tensorBuffer);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to create managed tensor buffer for output index {}", i);
            return false;
        }
        spdlog::info("Created output tensor buffer for index {}", i);
        outputTensorBuffers.push_back(tensorBuffer);
    }

    {
        void* hostMemAddr;
        const float kInput0Tensor[] = { 3, 256, 256 }; // Example input tensor data

        status = LiteRtLockTensorBuffer(inputTensorBuffers[0], &hostMemAddr);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to lock tensor buffer for input index 0");
            return false;
        }

        std::memcpy(hostMemAddr, kInput0Tensor, sizeof(kInput0Tensor));
        status = LiteRtUnlockTensorBuffer(inputTensorBuffers[0]);

        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to unlock tensor buffer for input index 0");
            return false;
        }
    }

    {
        status = LiteRtRunCompiledModel(
            m_compiledModel, /*signature_index=*/0,
            inputTensorBuffers.size(), inputTensorBuffers.data(),
            outputTensorBuffers.size(), outputTensorBuffers.data());
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to run compiled model");
            return false;
        }
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