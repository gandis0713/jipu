#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "file.h"
#include "image.h"

#include "litert/c/litert_common.h"
#include "litert/c/litert_compiled_model.h"
#include "litert/c/litert_environment.h"
#include "litert/c/litert_model.h"
#include "litert/c/litert_options.h"
#include "litert/c/litert_tensor_buffer.h"
#include "litert/c/litert_tensor_buffer_requirements.h"
#include "litert/c/litert_tensor_buffer_types.h"

namespace fs = std::filesystem;
namespace jipu
{

class LiteRtImageInference
{
public:
    LiteRtImageInference();
    ~LiteRtImageInference();

    bool setInputImage(Image* image);
    bool loadModel(const std::vector<char>& modelBuffer);
    std::vector<uint8_t> runInference();

    int32_t getBatchSize();
    int32_t getWidth();
    int32_t getHeight();
    int32_t getInputChannel();
    int32_t getOutputChannel();
    size_t getInputByteSize();
    size_t getInputSize();
    size_t getOutputByteSize();
    size_t getOutputSize();

private:
    void preprocessImage(std::vector<float>& preprocessed);

    std::vector<uint8_t> postprocessOutput(const float* output,
                                           int outputSize);

private:
    LiteRtModel m_model{ nullptr };
    LiteRtCompiledModel m_compiledModel{ nullptr };
    LiteRtOptions m_options{ nullptr };
    LiteRtEnvironment m_environment{ nullptr };
    LiteRtHwAcceleratorSet m_acceleratorType{ kLiteRtHwAcceleratorCpu };
    std::vector<LiteRtTensorBuffer> m_inputTensorBuffers{};
    std::vector<LiteRtTensorBuffer> m_outputTensorBuffers{};
    Image* m_inputImage{ nullptr };
};

} // namespace jipu