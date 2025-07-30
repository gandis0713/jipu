#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
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
#include "litert/c/options/litert_cpu_options.h"
#include "litert/c/options/litert_gpu_options.h"

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
// #include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <android/hardware_buffer.h>
// #include <android/native_window.h>
// #include <android/sync.h>

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
    bool nextFrame(EGLImageKHR image);

    int32_t getBatchSize();
    int32_t getWidth();
    int32_t getHeight();
    int32_t getInputChannel();
    int32_t getOutputChannel();
    size_t getInputByteSize();
    size_t getInputSize();
    size_t getOutputByteSize();
    size_t getOutputSize();

public:
    enum class AcceleratorType
    {
        kCPU = 0,
        kGPU,
        kNPU
    };

private:
    LiteRtOptions _createGpuOptions();
    LiteRtOptions _createCpuOptions();
    LiteRtHwAcceleratorSet _getAcceleratorTypeSet(AcceleratorType type);
    LiteRtRankedTensorType _getInputTensorType(LiteRtEnvironment environment,
                                               LiteRtCompiledModel compiledModel,
                                               LiteRtModel model,
                                               int signatureIndex,
                                               int inputIndex);
    LiteRtRankedTensorType _getInputTensorType(LiteRtEnvironment environment,
                                               LiteRtCompiledModel compiledModel,
                                               LiteRtModel model,
                                               int signatureIndex,
                                               std::string_view inputName);

    LiteRtRankedTensorType _getOutputTensorType(LiteRtEnvironment environment,
                                                LiteRtCompiledModel compiledModel,
                                                LiteRtModel model,
                                                int signatureIndex,
                                                int outputIndex);
    LiteRtRankedTensorType _getOutputTensorType(LiteRtEnvironment environment,
                                                LiteRtCompiledModel compiledModel,
                                                LiteRtModel model,
                                                int signatureIndex,
                                                std::string_view outputName);

    std::vector<LiteRtTensorBuffer> _createGLInputTensorBuffer(LiteRtEnvironment environment,
                                                               LiteRtModel model,
                                                               LiteRtCompiledModel compiledModel,
                                                               int signatureIndex);

    std::vector<LiteRtTensorBuffer> _createGLOutputTensorBuffer(LiteRtEnvironment environment,
                                                                LiteRtModel model,
                                                                LiteRtCompiledModel compiledModel,
                                                                int signatureIndex);

    std::vector<LiteRtSignature> _getSignatures(LiteRtEnvironment environment,
                                                LiteRtCompiledModel compiledModel,
                                                LiteRtModel model);

    void preprocessImage(std::vector<float>& preprocessed);

    std::vector<uint8_t> postprocessOutput(const float* output,
                                           int outputSize);

private:
    LiteRtModel m_model{ nullptr };
    LiteRtCompiledModel m_compiledModel{ nullptr };
    LiteRtOptions m_options{ nullptr };
    LiteRtOpaqueOptions m_gpuOptions{ nullptr };
    LiteRtEnvironment m_environment{ nullptr };
    AcceleratorType m_acceleratorType{ AcceleratorType::kCPU };
    std::vector<LiteRtTensorBuffer> m_inputTensorBuffers{};
    std::vector<LiteRtTensorBuffer> m_outputTensorBuffers{};
    std::vector<LiteRtSignature> m_signatures{};
    Image* m_inputImage{ nullptr };
    std::vector<float> m_preprocessed{};
    bool m_isUseGLBuffer{ false }; // Use GLBuffer for input/output
};

} // namespace jipu