#include "litert_image_inference.h"

#include <algorithm>
#include <cmath>
#include <dlfcn.h>
#include <spdlog/spdlog.h>

namespace jipu
{

namespace
{
std::string _getInputOutputStr(LiteRtImageInference::InputOutputType type)
{
    switch (type)
    {
    case LiteRtImageInference::InputOutputType::kInput:
        return "input";
    case LiteRtImageInference::InputOutputType::kOutput:
        return "output";
    default:
        return "unknown";
    }
}
} // namespace

LiteRtImageInference::LiteRtImageInference(EGLContext context, EGLDisplay display)
    : m_model(nullptr)
    , m_inputImage(nullptr)
    , m_context(context)
    , m_display(display)
{
    const char* libOpenCLPath = "libOpenCL.so"; // Adjust this path as needed
    if (auto libopenCL = dlopen(libOpenCLPath, RTLD_NOW | RTLD_GLOBAL))
    {
        spdlog::info("Successfully loaded OpenCL library: {}", libOpenCLPath);

        m_acceleratorType = AcceleratorType::kGPU;

        dlclose(libopenCL);
    }
    else
    {
        m_acceleratorType = AcceleratorType::kCPU;
        spdlog::error("Failed to load OpenCL library: {}", dlerror());
    }
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

    preprocessImage(m_preprocessed);
    if (m_preprocessed.empty())
    {
        spdlog::error("Preprocessed data is empty");
        return false;
    }

    return true;
}

bool LiteRtImageInference::loadModel(const std::vector<char>& modelBuffer)
{
    LiteRtStatus status = LiteRtCreateModelFromBuffer(modelBuffer.data(), modelBuffer.size(), &m_model);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create model from buffer {}", static_cast<uint32_t>(status));
        return false;
    }

    switch (m_acceleratorType)
    {
    case AcceleratorType::kGPU:
        m_options = _createGpuOptions();
        break;
    case AcceleratorType::kCPU:
        m_options = _createCpuOptions();
        break;
    default:
        spdlog::error("Unsupported accelerator type");
        return false;
    }

    if (m_options == nullptr)
    {
        spdlog::error("Failed to create options");
        return false;
    }

    std::vector<LiteRtEnvOption> options{};
    if (m_acceleratorType == AcceleratorType::kGPU)
    {
        options.push_back({ .tag = kLiteRtEnvOptionTagEglContext,
                            .value = { .type = kLiteRtAnyTypeInt, .int_value = reinterpret_cast<int64_t>(m_context) } });
        options.push_back({ .tag = kLiteRtEnvOptionTagEglDisplay,
                            .value = { .type = kLiteRtAnyTypeInt, .int_value = reinterpret_cast<int64_t>(m_display) } });
    }

    status = LiteRtCreateEnvironment(options.size(), options.data(), &m_environment);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create environment {}", static_cast<uint32_t>(status));
        return false;
    }

    status = LiteRtCreateCompiledModel(m_environment, m_model, m_options, &m_compiledModel);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create compiled model {}", static_cast<uint32_t>(status));
        return false;
    }

    m_signatures = _getSignatures(m_model);
    if (m_signatures.empty())
    {
        spdlog::error("Failed to get model signatures");
        return false;
    }

    // create gl input buffers
    if (m_acceleratorType == AcceleratorType::kGPU)
    {
        m_inputTensorBuffers = _createGLTensorBuffer(
            m_environment, m_model, m_compiledModel, m_signatureIndex, InputOutputType::kInput);
        if (m_inputTensorBuffers.empty())
        {
            spdlog::error("Failed to create GL input tensor buffers");
            return false;
        }

        m_outputTensorBuffers = _createGLTensorBuffer(
            m_environment, m_model, m_compiledModel, m_signatureIndex, InputOutputType::kOutput);
        if (m_outputTensorBuffers.empty())
        {
            spdlog::error("Failed to create GL output tensor buffers");
            return false;
        }
    }
    else if (m_acceleratorType == AcceleratorType::kCPU)
    {
        //        LiteRtLayout layout = { 1, false, { getInputChannel(), getInputHeight(), getInputWidth() }, {} };
        //        LiteRtRankedTensorType kInput0TensorType{ .element_type = kLiteRtElementTypeFloat32,
        //                                                  .layout = layout };
        //
        //        m_inputTensorBuffers.clear();
        //        m_inputTensorBuffers.reserve(numInputs);
        //        for (auto i = 0; i < numInputs; ++i)
        //        {
        //            LiteRtTensorBufferRequirements tensorBufferRequirements;
        //            status = LiteRtGetCompiledModelInputBufferRequirements(
        //                m_compiledModel, /*signature_index=*/0, i,
        //                &tensorBufferRequirements);
        //
        //            if (status != kLiteRtStatusOk)
        //            {
        //                spdlog::error("Failed to get input buffer requirements for index {}, status: {}", i, static_cast<uint32_t>(status));
        //                return false;
        //            }
        //
        //            LiteRtTensorBufferType tensorBufferType;
        //            status = LiteRtGetTensorBufferRequirementsSupportedTensorBufferType(
        //                tensorBufferRequirements, /*type_index=*/0, &tensorBufferType);
        //            if (status != kLiteRtStatusOk)
        //            {
        //                spdlog::error("Failed to get tensor buffer type {}", static_cast<uint32_t>(status));
        //                return false;
        //            }
        //
        //            size_t tensorBufferSize;
        //            status = LiteRtGetTensorBufferRequirementsBufferSize(
        //                tensorBufferRequirements, &tensorBufferSize);
        //            if (status != kLiteRtStatusOk)
        //            {
        //                spdlog::error("Failed to get tensor buffer size {}", static_cast<uint32_t>(status));
        //                return false;
        //            }
        //
        //            spdlog::info("Creating input tensor buffer for index {} with size {}", i, tensorBufferSize);
        //
        //            LiteRtTensorBuffer tensorBuffer;
        //            status = LiteRtCreateManagedTensorBuffer(
        //                m_environment, tensorBufferType, &kInput0TensorType,
        //                tensorBufferSize, &tensorBuffer);
        //            if (status != kLiteRtStatusOk)
        //            {
        //                spdlog::error("Failed to create managed tensor buffer for input index {}, status: {}", i, static_cast<uint32_t>(status));
        //                return false;
        //            }
        //
        //            m_inputTensorBuffers.push_back(tensorBuffer);
        //        }
        //
        //        spdlog::info("Model loaded with {} input tensor buffers", m_inputTensorBuffers.size());
        //
        //        LiteRtParamIndex numOutputs;
        //        status = LiteRtGetNumSubgraphOutputs(subgraph, &numOutputs);
        //        if (status != kLiteRtStatusOk)
        //        {
        //            spdlog::error("Failed to get number of subgraph outputs {}", static_cast<uint32_t>(status));
        //            return false;
        //        }
        //
        //        spdlog::info("Model loaded with {} output tensors", numOutputs);
        //
        //        m_outputTensorBuffers.clear();
        //        m_outputTensorBuffers.reserve(numOutputs);
        //        for (auto i = 0; i < numOutputs; ++i)
        //        {
        //            LiteRtTensorBufferRequirements tensorBufferRequirements;
        //            status = LiteRtGetCompiledModelOutputBufferRequirements(
        //                m_compiledModel, /*signature_index=*/0, i,
        //                &tensorBufferRequirements);
        //            if (status != kLiteRtStatusOk)
        //            {
        //                spdlog::error("Failed to get output buffer requirements for index {}, status: {}", i, static_cast<uint32_t>(status));
        //                return false;
        //            }
        //
        //            LiteRtTensorBufferType tensorBufferType;
        //            status = LiteRtGetTensorBufferRequirementsSupportedTensorBufferType(
        //                tensorBufferRequirements, /*type_index=*/0, &tensorBufferType);
        //            size_t tensorBufferSize;
        //            if (status != kLiteRtStatusOk)
        //            {
        //                spdlog::error("Failed to get tensor buffer type for output index {}, status: {}", i, static_cast<uint32_t>(status));
        //                return false;
        //            }
        //
        //            status = LiteRtGetTensorBufferRequirementsBufferSize(
        //                tensorBufferRequirements, &tensorBufferSize);
        //            if (status != kLiteRtStatusOk)
        //            {
        //                spdlog::error("Failed to get tensor buffer size for output index {}, status: {}", i, static_cast<uint32_t>(status));
        //                return false;
        //            }
        //
        //            spdlog::info("Creating output tensor buffer for index {} with size {}", i, tensorBufferSize);
        //
        //            LiteRtTensorBuffer tensorBuffer;
        //            status = LiteRtCreateManagedTensorBuffer(
        //                m_environment, tensorBufferType, &kInput0TensorType,
        //                tensorBufferSize, &tensorBuffer);
        //            if (status != kLiteRtStatusOk)
        //            {
        //                spdlog::error("Failed to create managed tensor buffer for output index {}, status: {}", i, static_cast<uint32_t>(status));
        //                return false;
        //            }
        //            spdlog::info("Created output tensor buffer for index {}", i);
        //            m_outputTensorBuffers.push_back(tensorBuffer);
        //        }
    }
    else
    {
        spdlog::error("Unsupported accelerator type");
        return false;
    }

    spdlog::info("Model loaded successfully");
    return true;
}

int32_t LiteRtImageInference::getInputBatchSize()
{
    return 1; // Assuming batch size of 1 for simplicity
}

int32_t LiteRtImageInference::getInputHeight()
{
    return 256;
}

int32_t LiteRtImageInference::getInputWidth()
{
    return 256;
}

int32_t LiteRtImageInference::getInputChannel()
{
    return 3;
}

int32_t LiteRtImageInference::getOutputBatchSize()
{
    return 1; // Assuming batch size of 1 for simplicity
}

int32_t LiteRtImageInference::getOutputHeight()
{
    return 256;
}

int32_t LiteRtImageInference::getOutputWidth()
{
    return 256;
}

int32_t LiteRtImageInference::getOutputChannel()
{
    return 1;
}

size_t LiteRtImageInference::getInputByteSize()
{
    return getInputSize() * sizeof(float);
}

size_t LiteRtImageInference::getInputSize()
{
    return getInputChannel() * getInputHeight() * getInputWidth();
}

size_t LiteRtImageInference::getOutputByteSize()
{
    return getOutputSize() * sizeof(float);
}

size_t LiteRtImageInference::getOutputSize()
{
    return getOutputChannel() * getOutputHeight() * getOutputWidth();
}

LiteRt_GlBuffer LiteRtImageInference::getInputGlBuffer()
{
    if (m_inputTensorBuffers.empty())
    {
        spdlog::error("Input tensor buffers are empty");
        return {};
    }

    LiteRt_GlBuffer glBuffer{};
    LiteRtStatus status = LiteRtGetTensorBufferGlBuffer(m_inputTensorBuffers[0], &glBuffer.target, &glBuffer.id, &glBuffer.size, &glBuffer.offset);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get GL buffer from input tensor buffer, status: {}", static_cast<uint32_t>(status));
        return {};
    }

    return glBuffer;
}

LiteRt_GlBuffer LiteRtImageInference::getOutputGlBuffer()
{
    if (m_outputTensorBuffers.empty())
    {
        spdlog::error("Output tensor buffers are empty");
        return {};
    }

    LiteRt_GlBuffer glBuffer{};
    LiteRtStatus status = LiteRtGetTensorBufferGlBuffer(m_outputTensorBuffers[0], &glBuffer.target, &glBuffer.id, &glBuffer.size, &glBuffer.offset);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get GL buffer from output tensor buffer, status: {}", static_cast<uint32_t>(status));
        return {};
    }

    return glBuffer;
}

std::vector<float> LiteRtImageInference::getPreprocessedData() const
{
    return m_preprocessed;
}

LiteRtOptions LiteRtImageInference::_createGpuOptions()
{
    // create and set GPU options
    LiteRtStatus status = LiteRtCreateGpuOptions(&m_gpuOptions);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create GPU options {}", static_cast<uint32_t>(status));
        return nullptr;
    }

    if (m_isUseGLBuffer)
    {
        LiteRtSetGpuAcceleratorCompilationOptionsPrecision(m_gpuOptions, kLiteRtDelegatePrecisionFp32);
        LiteRtSetGpuAcceleratorCompilationOptionsUseBufferStorageType(m_gpuOptions, kLiteRtDelegateBufferStorageTypeBuffer);
        LiteRtSetGpuOptionsNoImmutableExternalTensorsMode(m_gpuOptions, false);
    }
    else
    {
        LiteRtSetGpuOptionsNoImmutableExternalTensorsMode(m_gpuOptions, true);
    }

    // create and set options
    LiteRtOptions options = nullptr;
    status = LiteRtCreateOptions(&options);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create options {}", static_cast<uint32_t>(status));
        return nullptr;
    }

    status = LiteRtSetOptionsHardwareAccelerators(options, _getAcceleratorTypeSet(m_acceleratorType));
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to set hardware accelerators {}", static_cast<uint32_t>(status));
        return nullptr;
    }

    status = LiteRtAddOpaqueOptions(options, m_gpuOptions);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to add opaque options {}", static_cast<uint32_t>(status));
        return nullptr;
    }

    return options;
}

LiteRtOptions LiteRtImageInference::_createCpuOptions()
{
    // create and set options
    LiteRtOptions options = nullptr;
    LiteRtStatus status = LiteRtCreateOptions(&options);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to create options {}", static_cast<uint32_t>(status));
        return nullptr;
    }

    status = LiteRtSetOptionsHardwareAccelerators(options, _getAcceleratorTypeSet(m_acceleratorType));
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to set hardware accelerators {}", static_cast<uint32_t>(status));
        return nullptr;
    }

    return options;
}

LiteRtHwAcceleratorSet LiteRtImageInference::_getAcceleratorTypeSet(AcceleratorType type)
{
    switch (type)
    {
    case AcceleratorType::kCPU:
        return kLiteRtHwAcceleratorCpu;
    case AcceleratorType::kGPU:
        return kLiteRtHwAcceleratorGpu;
    case AcceleratorType::kNPU:
        return kLiteRtHwAcceleratorNpu;
    default:
        spdlog::error("Unknown accelerator type");
        return kLiteRtHwAcceleratorCpu; // Fallback to CPU
    }
}

LiteRtRankedTensorType LiteRtImageInference::_getTensorType(
    LiteRtModel model,
    int signatureIndex,
    int index,
    InputOutputType type)
{
    LiteRtSubgraph subgraph;
    LiteRtStatus status = LiteRtGetModelSubgraph(model, signatureIndex, &subgraph);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get model subgraph to get {} tensor type {}", _getInputOutputStr(type), static_cast<uint32_t>(status));
        return {};
    }

    LiteRtParamIndex subgraphNumInputs;

    switch (type)
    {
    case InputOutputType::kInput:
        status = LiteRtGetNumSubgraphInputs(subgraph, &subgraphNumInputs);
        break;
    case InputOutputType::kOutput:
        status = LiteRtGetNumSubgraphOutputs(subgraph, &subgraphNumInputs);
        break;
    default:
        spdlog::error("Unknown InputOutputType");
        return {};
    }

    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get number of subgraph for {} tensor type {}", _getInputOutputStr(type), static_cast<uint32_t>(status));
        return {};
    }

    std::vector<LiteRtTensor> tensors;
    for (auto i = 0; i < subgraphNumInputs; ++i)
    {
        LiteRtTensor tensor;
        switch (type)
        {
        case InputOutputType::kInput:
            status = LiteRtGetSubgraphInput(subgraph, i, &tensor);
            break;
        case InputOutputType::kOutput:
            status = LiteRtGetSubgraphOutput(subgraph, i, &tensor);
            break;
        default:
            spdlog::error("Unknown InputOutputType");
            return {};
        }

        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get subgraph {} {}", _getInputOutputStr(type), static_cast<uint32_t>(status));
            return {};
        }

        tensors.push_back(tensor);
    }

    LiteRtRankedTensorType rankedTensorType;
    status = LiteRtGetRankedTensorType(tensors[index], &rankedTensorType);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get ranked {} tensor type {}", _getInputOutputStr(type), static_cast<uint32_t>(status));
        return {};
    }

    spdlog::info("Ranked {} Tensor Type", _getInputOutputStr(type));
    spdlog::info("  Element Type: {}", static_cast<uint32_t>(rankedTensorType.element_type));
    spdlog::info("  Layout");
    spdlog::info("    has_strides: {}", static_cast<bool>(rankedTensorType.layout.has_strides));
    spdlog::info("    rank: {}", static_cast<uint32_t>(rankedTensorType.layout.rank));
    spdlog::info("    dimensions: [");
    for (auto i = 0; i < rankedTensorType.layout.rank; ++i)
    {
        spdlog::info("      {}", rankedTensorType.layout.dimensions[i]);
    }
    spdlog::info("    ]");
    spdlog::info("    strides: [");
    for (auto i = 0; i < rankedTensorType.layout.rank; ++i)
    {
        spdlog::info("      {}", rankedTensorType.layout.strides[i]);
    }
    spdlog::info("    ]");

    return rankedTensorType;
}
LiteRtRankedTensorType LiteRtImageInference::_getTensorType(
    LiteRtModel model,
    int signatureIndex,
    std::string_view name,
    InputOutputType type)
{
    LiteRtSubgraph subgraph;
    LiteRtStatus status = LiteRtGetModelSubgraph(model, signatureIndex, &subgraph);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get model subgraph to get {} tensor type {}", _getInputOutputStr(type), static_cast<uint32_t>(status));
        return {};
    }

    LiteRtParamIndex subgraphNumInputs;

    switch (type)
    {
    case InputOutputType::kInput:
        status = LiteRtGetNumSubgraphInputs(subgraph, &subgraphNumInputs);
        break;
    case InputOutputType::kOutput:
        status = LiteRtGetNumSubgraphOutputs(subgraph, &subgraphNumInputs);
        break;
    default:
        spdlog::error("Unknown InputOutputType");
        return {};
    }

    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get number of subgraph for {} tensor type {}", _getInputOutputStr(type), static_cast<uint32_t>(status));
        return {};
    }

    for (size_t i = 0; i < subgraphNumInputs; ++i)
    {
        LiteRtTensor tensor;
        switch (type)
        {
        case InputOutputType::kInput:
            status = LiteRtGetSubgraphInput(subgraph, i, &tensor);
            break;
        case InputOutputType::kOutput:
            status = LiteRtGetSubgraphOutput(subgraph, i, &tensor);
            break;
        default:
            spdlog::error("Unknown InputOutputType");
            return {};
        }

        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get subgraph {} {}", _getInputOutputStr(type), static_cast<uint32_t>(status));
            return {};
        }

        const char* tensorName;
        status = LiteRtGetTensorName(tensor, &tensorName);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor name {}", static_cast<uint32_t>(status));
            return {};
        }

        if (name == std::string_view(tensorName))
        {
            LiteRtRankedTensorType rankedTensorType;
            status = LiteRtGetRankedTensorType(tensor, &rankedTensorType);
            if (status != kLiteRtStatusOk)
            {
                spdlog::error("Failed to get ranked {} tensor type {}", _getInputOutputStr(type), static_cast<uint32_t>(status));
                return {};
            }

            spdlog::info("Ranked {} Tensor Type", _getInputOutputStr(type));
            spdlog::info("  Element Type: {}", static_cast<uint32_t>(rankedTensorType.element_type));
            spdlog::info("Layout");
            spdlog::info("  has_strides: {}", static_cast<bool>(rankedTensorType.layout.has_strides));
            spdlog::info("  rank: {}", static_cast<uint32_t>(rankedTensorType.layout.rank));
            spdlog::info("  dimensions: [");
            for (auto i = 0; i < rankedTensorType.layout.rank; ++i)
            {
                spdlog::info("    {}", rankedTensorType.layout.dimensions[i]);
            }
            spdlog::info("  ]");
            spdlog::info("  strides: [");
            for (auto i = 0; i < rankedTensorType.layout.rank; ++i)
            {
                spdlog::info("    {}", rankedTensorType.layout.strides[i]);
            }
            spdlog::info("  ]");
            spdlog::info("}}");
        }
    }
    return {};
}

std::vector<LiteRtTensorBuffer> LiteRtImageInference::_createGLTensorBuffer(LiteRtEnvironment environment,
                                                                            LiteRtModel model,
                                                                            LiteRtCompiledModel compiledModel,
                                                                            int signatureIndex,
                                                                            InputOutputType type)
{
    if (m_acceleratorType == AcceleratorType::kCPU)
    {
        spdlog::error("GL tensor buffers are not supported for CPU accelerator type");
        return {};
    }

    std::vector<LiteRtTensorBuffer> tensorBuffers;

    std::vector<LiteRtTensorBufferRequirements> tensorBufferRequirementsList = _getTensorBufferRequirementsList(compiledModel, signatureIndex, type);
    for (LiteRtParamIndex i = 0; i < tensorBufferRequirementsList.size(); ++i)
    {
        LiteRtTensorBufferRequirements tensorBufferRequirements = tensorBufferRequirementsList[i];
        std::vector<LiteRtTensorBufferType> supportedBufferTypes = _getSupportedTensorBufferTypes(tensorBufferRequirements);

        bool isContainedGlBufferType = std::find(supportedBufferTypes.begin(), supportedBufferTypes.end(), kLiteRtTensorBufferTypeGlBuffer) != supportedBufferTypes.end();
        if (!isContainedGlBufferType)
        {
            spdlog::error("kLiteRtTensorBufferTypeGlBuffer is not supported for {} index {}, signature index {}", _getInputOutputStr(type), i, signatureIndex);
            return {};
        }

        LiteRtRankedTensorType rankedTensorType = _getTensorType(model, signatureIndex, i, type);

        size_t tensorBufferSize;
        LiteRtStatus status = LiteRtGetTensorBufferRequirementsBufferSize(
            tensorBufferRequirements, &tensorBufferSize);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor buffer size {}", static_cast<uint32_t>(status));
            return {};
        }

        spdlog::info("Creating {} tensor buffer for signature index {}, index {} with size {}",
                     _getInputOutputStr(type), signatureIndex, i, tensorBufferSize);

        LiteRtTensorBuffer tensorBuffer;
        status = LiteRtCreateManagedTensorBuffer(environment, kLiteRtTensorBufferTypeGlBuffer, &rankedTensorType, tensorBufferSize, &tensorBuffer);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to create tensor buffer {}", static_cast<uint32_t>(status));
            return {};
        }

        tensorBuffers.push_back(tensorBuffer);
    }

    return tensorBuffers;
}

std::vector<LiteRtTensorBuffer> LiteRtImageInference::_createTensorBuffer(LiteRtEnvironment environment,
                                                                          LiteRtModel model,
                                                                          LiteRtCompiledModel compiledModel,
                                                                          int signatureIndex,
                                                                          InputOutputType type)
{
    std::vector<LiteRtTensorBuffer> tensorBuffers;

    std::vector<LiteRtTensorBufferRequirements> tensorBufferRequirementsList = _getTensorBufferRequirementsList(compiledModel, signatureIndex, type);
    for (LiteRtParamIndex i = 0; i < tensorBufferRequirementsList.size(); ++i)
    {
        LiteRtTensorBufferRequirements tensorBufferRequirements = tensorBufferRequirementsList[i];
        LiteRtRankedTensorType rankedTensorType = _getTensorType(model, signatureIndex, i, type);
        std::vector<LiteRtTensorBufferType> supportedBufferTypes = _getSupportedTensorBufferTypes(tensorBufferRequirements);

        // TODO
    }

    return tensorBuffers;
}

std::vector<LiteRtSignature> LiteRtImageInference::_getSignatures(LiteRtModel model)
{
    LiteRtParamIndex numSignatures;
    LiteRtGetNumModelSignatures(model, &numSignatures);

    std::vector<LiteRtSignature> signatures{};
    signatures.resize(numSignatures);
    for (LiteRtParamIndex i = 0; i < numSignatures; ++i)
    {
        LiteRtSignature liteRtSignature;
        LiteRtGetModelSignature(model, i, &liteRtSignature);

        signatures[i] = liteRtSignature;
    }

    return signatures;
}

LiteRtParamIndex LiteRtImageInference::_getNumSignatureInOuts(LiteRtSignature signature, InputOutputType type)
{
    LiteRtParamIndex numInputOutputs;

    LiteRtStatus status = kLiteRtStatusOk;
    switch (type)
    {
    case InputOutputType::kInput:
        status = LiteRtGetNumSignatureInputs(signature, &numInputOutputs);
        break;
    case InputOutputType::kOutput:
        status = LiteRtGetNumSignatureOutputs(signature, &numInputOutputs);
        break;
    default:
        spdlog::error("Unknown InputOutputType");
        return {};
    }

    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get number of signature {}s, status: {}",
                      _getInputOutputStr(type), static_cast<uint32_t>(status));
    }

    return numInputOutputs;
}

std::vector<LiteRtTensorBufferRequirements> LiteRtImageInference::_getTensorBufferRequirementsList(LiteRtCompiledModel compiledModel,
                                                                                                   int signatureIndex,
                                                                                                   InputOutputType type)
{
    std::vector<LiteRtTensorBufferRequirements> tensorBufferRequirementsList;

    LiteRtSignature signature = m_signatures[signatureIndex];
    LiteRtParamIndex numInputOutputs = _getNumSignatureInOuts(signature, type);

    for (LiteRtParamIndex i = 0; i < numInputOutputs; ++i)
    {
        LiteRtTensorBufferRequirements tensorBufferRequirements;

        LiteRtStatus status = kLiteRtStatusErrorUnknown;
        switch (type)
        {
        case InputOutputType::kInput:
            status = LiteRtGetCompiledModelInputBufferRequirements(
                compiledModel, signatureIndex, i, &tensorBufferRequirements);
            break;
        case InputOutputType::kOutput:
            status = LiteRtGetCompiledModelOutputBufferRequirements(
                compiledModel, signatureIndex, i, &tensorBufferRequirements);
            break;
        default:
            spdlog::error("Unknown InputOutputType");
            break;
        }

        // Check if the status is not ok
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get {} buffer requirements for signature index {}, index {}, status: {}",
                          _getInputOutputStr(type), signatureIndex, i, static_cast<uint32_t>(status));
            continue;
        }

        tensorBufferRequirementsList.push_back(tensorBufferRequirements);
    }

    if (tensorBufferRequirementsList.empty())
    {
        spdlog::error("No tensor buffer requirements found for signature index {}, type {}", signatureIndex, _getInputOutputStr(type));
    }

    return tensorBufferRequirementsList;
}

std::vector<LiteRtTensorBufferType> LiteRtImageInference::_getSupportedTensorBufferTypes(LiteRtTensorBufferRequirements tensorBufferRequirements)
{
    std::vector<LiteRtTensorBufferType> supportedBufferTypes{};

    int numSupportedBufferTypes;
    LiteRtStatus status = LiteRtGetNumTensorBufferRequirementsSupportedBufferTypes(tensorBufferRequirements, &numSupportedBufferTypes);
    if (status == kLiteRtStatusOk)
    {
        for (int i = 0; i < numSupportedBufferTypes; ++i)
        {
            LiteRtTensorBufferType supportedType;
            status = LiteRtGetTensorBufferRequirementsSupportedTensorBufferType(tensorBufferRequirements, i, &supportedType);
            if (status != kLiteRtStatusOk)
            {
                spdlog::error("Failed to get supported tensor buffer type for type index {}, status: {}", i, static_cast<uint32_t>(status));
                continue;
            }
            spdlog::info("Supported tensor buffer type for type index {}: {}", i, static_cast<uint32_t>(supportedType));

            supportedBufferTypes.push_back(supportedType);
        }
    }

    if (supportedBufferTypes.empty())
    {
        spdlog::error("No supported tensor buffer types found");
    }

    return supportedBufferTypes;
}

void LiteRtImageInference::preprocessImage(std::vector<float>& preprocessed)
{
    size_t intputBatchSize = getInputBatchSize();
    size_t inputSize = getInputSize();
    int32_t inputWidth = getInputWidth();
    int32_t inputHeight = getInputHeight();
    int32_t inputChannels = getInputChannel();
    size_t inputByteSize = getInputByteSize();

    spdlog::info("Expected input shape: [{}, {}, {}, {}], byte size:[{}], inputSize:[{}]",
                 intputBatchSize, inputHeight, inputWidth, inputChannels, inputByteSize, inputSize);

    preprocessed.clear();
    preprocessed.resize(inputSize);

    auto inputImage = static_cast<uint8_t*>(m_inputImage->getPixels());
    int imgWidth = m_inputImage->getWidth();
    int imgHeight = m_inputImage->getHeight();

    for (int y = 0; y < inputHeight; y++)
    {
        for (int x = 0; x < inputWidth; x++)
        {
            // 이미지 리사이징 및 정규화
            int srcX = x * imgWidth / inputWidth;
            int srcY = y * imgHeight / inputHeight;

            for (int c = 0; c < inputChannels; c++)
            {
                int srcIdx = (srcY * imgWidth + srcX) * inputChannels + c;
                int dstIdx = (y * inputWidth + x) * inputChannels + c;

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
    //    void* hostInputMemAddr;
    //    {
    //        auto status = LiteRtLockTensorBuffer(m_inputTensorBuffers[0], &hostInputMemAddr);
    //        if (status != kLiteRtStatusOk)
    //        {
    //            spdlog::error("Failed to lock tensor buffer for input index 0, status: {}", static_cast<uint32_t>(status));
    //            return {};
    //        }
    //
    //        std::memcpy(hostInputMemAddr, m_preprocessed.data(), m_preprocessed.size() * sizeof(float));
    //        spdlog::info("Copied preprocessed data to input tensor buffer, size: {} bytes",
    //                     m_preprocessed.size() * sizeof(float));
    //
    //        status = LiteRtUnlockTensorBuffer(m_inputTensorBuffers[0]);
    //        if (status != kLiteRtStatusOk)
    //        {
    //            spdlog::error("Failed to unlock tensor buffer for input index 0, status: {}", static_cast<uint32_t>(status));
    //            return {};
    //        }
    //    }

    auto status = LiteRtRunCompiledModel(
        m_compiledModel, m_signatureIndex,
        m_inputTensorBuffers.size(), m_inputTensorBuffers.data(),
        m_outputTensorBuffers.size(), m_outputTensorBuffers.data());
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to run compiled model");
        return {};
    }

    void* hostOutputMemAddr;
    {
        status = LiteRtLockTensorBuffer(m_outputTensorBuffers[0], &hostOutputMemAddr);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to lock output tensor buffer");
            return {};
        }

        status = LiteRtUnlockTensorBuffer(m_outputTensorBuffers[0]);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to unlock output tensor buffer");
            return {};
        }
    }

    return postprocessOutput(
        static_cast<const float*>(hostOutputMemAddr),
        getOutputSize());
}

bool LiteRtImageInference::nextFrame(EGLImageKHR image)
{
    if (!image)
    {
        spdlog::error("EGLImageKHR is null");
        return false;
    }

    // Process the next frame using the provided EGLImageKHR
    // ...

    return true;
}

} // namespace jipu