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

struct LiteRt_GlBuffer
{
    LiteRtGLenum target;
    LiteRtGLuint id;
    size_t size; // Size in bytes
    size_t offset;
};

class LiteRtImageInference
{
public:
    LiteRtImageInference(EGLContext context, EGLDisplay display);
    ~LiteRtImageInference();

    bool setInputImage(Image* image);
    bool loadModel(const std::vector<char>& modelBuffer);
    std::vector<uint8_t> runInference();
    bool nextFrame(EGLImageKHR image);

    int32_t getInputBatchSize();
    int32_t getInputHeight();
    int32_t getInputWidth();
    int32_t getInputChannel();
    int32_t getOutputBatchSize();
    int32_t getOutputHeight();
    int32_t getOutputWidth();
    int32_t getOutputChannel();

    size_t getInputByteSize();
    size_t getInputSize();
    size_t getOutputByteSize();
    size_t getOutputSize();

    LiteRt_GlBuffer getInputGlBuffer();
    LiteRt_GlBuffer getOutputGlBuffer();

    std::vector<float> getPreprocessedData() const;

public:
    enum class AcceleratorType
    {
        kCPU = 0,
        kGPU,
        kNPU
    };

    enum class InputOutputType
    {
        kInput = 0,
        kOutput
    };

private:
    LiteRtOptions _createGpuOptions();
    LiteRtOptions _createCpuOptions();
    LiteRtHwAcceleratorSet _getAcceleratorTypeSet(AcceleratorType type);
    LiteRtRankedTensorType _getTensorType(LiteRtModel model,
                                          int signatureIndex,
                                          int index,
                                          InputOutputType type);
    LiteRtRankedTensorType _getTensorType(LiteRtModel model,
                                          int signatureIndex,
                                          std::string_view name,
                                          InputOutputType type);
    std::vector<LiteRtTensorBuffer> _createGLTensorBuffer(LiteRtEnvironment environment,
                                                          LiteRtModel model,
                                                          LiteRtCompiledModel compiledModel,
                                                          int signatureIndex,
                                                          InputOutputType type);
    std::vector<LiteRtTensorBuffer> _createTensorBuffer(LiteRtEnvironment environment,
                                                        LiteRtModel model,
                                                        LiteRtCompiledModel compiledModel,
                                                        int signatureIndex,
                                                        InputOutputType type);

    std::vector<LiteRtSignature> _getSignatures(LiteRtModel model);
    LiteRtParamIndex _getNumSignatureInOuts(LiteRtSignature signature,
                                            InputOutputType type);
    std::vector<LiteRtTensorBufferRequirements> _getTensorBufferRequirementsList(LiteRtCompiledModel compiledModel,
                                                                                 int signatureIndex,
                                                                                 InputOutputType type);
    std::vector<LiteRtTensorBufferType> _getSupportedTensorBufferTypes(LiteRtTensorBufferRequirements tensorBufferRequirements);

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
    int m_signatureIndex{ 0 };
    Image* m_inputImage{ nullptr };
    std::vector<float> m_preprocessed{};
    bool m_isUseGLBuffer{ false }; // Use GLBuffer for input/output

    EGLContext m_context{ nullptr };
    EGLDisplay m_display{ nullptr };
};

} // namespace jipu