#include "litert_image_inference.h"

#include <algorithm>
#include <cmath>
#include <dlfcn.h>
#include <spdlog/spdlog.h>

namespace jipu
{

LiteRtImageInference::LiteRtImageInference()
    : m_model(nullptr)
    , m_inputImage(nullptr)
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

    status = LiteRtCreateEnvironment(0, nullptr, &m_environment);
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

    m_signatures = _getSignatures(m_environment, m_compiledModel, m_model);
    if (m_signatures.empty())
    {
        spdlog::error("Failed to get model signatures");
        return false;
    }

    // create gl input buffers
    if (m_acceleratorType == AcceleratorType::kGPU)
    {
        size_t signatureIndex = 0;
        m_inputTensorBuffers = _createGLInputTensorBuffer(
            m_environment, m_model, m_compiledModel, signatureIndex);
        if (m_inputTensorBuffers.empty())
        {
            spdlog::error("Failed to create GL input tensor buffers");
            return false;
        }

        m_outputTensorBuffers = _createGLOutputTensorBuffer(
            m_environment, m_model, m_compiledModel, signatureIndex);
        if (m_outputTensorBuffers.empty())
        {
            spdlog::error("Failed to create GL output tensor buffers");
            return false;
        }
    }
    else if (m_acceleratorType == AcceleratorType::kCPU)
    {
        //        LiteRtLayout layout = { 1, false, { getInputChannel(), getHeight(), getWidth() }, {} };
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

int32_t LiteRtImageInference::getBatchSize()
{
    return 1; // Assuming batch size of 1 for simplicity
}

int32_t LiteRtImageInference::getWidth()
{
    return 256;
}

int32_t LiteRtImageInference::getHeight()
{
    return 256;
}

int32_t LiteRtImageInference::getInputChannel()
{
    return 3; // Assuming RGB input
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
    return getInputChannel() * getHeight() * getWidth();
}

size_t LiteRtImageInference::getOutputByteSize()
{
    return getOutputSize() * sizeof(float);
}

size_t LiteRtImageInference::getOutputSize()
{
    return getOutputChannel() * getHeight() * getWidth();
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

LiteRtRankedTensorType LiteRtImageInference::_getInputTensorType(LiteRtEnvironment environment,
                                                                 LiteRtCompiledModel compiledModel,
                                                                 LiteRtModel model,
                                                                 int signatureIndex,
                                                                 int inputIndex)
{
    LiteRtSubgraph subgraph;
    LiteRtStatus status = LiteRtGetModelSubgraph(m_model, signatureIndex, &subgraph);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get model subgraph {}", static_cast<uint32_t>(status));
        return {};
    }

    LiteRtParamIndex subgraphNumInputs;
    LiteRtGetNumSubgraphInputs(subgraph, &subgraphNumInputs);

    std::vector<LiteRtTensor> inputs;
    for (auto i = 0; i < subgraphNumInputs; ++i)
    {
        LiteRtTensor input;
        LiteRtGetSubgraphInput(subgraph, i, &input);
        inputs.push_back(input);
    }

    LiteRtRankedTensorType rankedTensorType;
    status = LiteRtGetRankedTensorType(inputs[inputIndex], &rankedTensorType);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get ranked tensor type {}", static_cast<uint32_t>(status));
        return {};
    }

    return rankedTensorType;
}

LiteRtRankedTensorType LiteRtImageInference::_getInputTensorType(LiteRtEnvironment environment,
                                                                 LiteRtCompiledModel compiledModel,
                                                                 LiteRtModel model,
                                                                 int signatureIndex,
                                                                 std::string_view inputName)
{
    LiteRtSubgraph subgraph;
    LiteRtStatus status = LiteRtGetModelSubgraph(m_model, signatureIndex, &subgraph);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get model subgraph {}", static_cast<uint32_t>(status));
        return {};
    }

    LiteRtParamIndex subgraphNumInputs;
    status = LiteRtGetNumSubgraphInputs(subgraph, &subgraphNumInputs);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get number of subgraph inputs {}", static_cast<uint32_t>(status));
        return {};
    }

    for (size_t i = 0; i < subgraphNumInputs; ++i)
    {
        LiteRtTensor inputTensor;
        status = LiteRtGetSubgraphInput(subgraph, i, &inputTensor);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get subgraph input {}", static_cast<uint32_t>(status));
            return {};
        }

        const char* tensorName;
        status = LiteRtGetTensorName(inputTensor, &tensorName);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor name {}", static_cast<uint32_t>(status));
            return {};
        }

        if (inputName == tensorName)
        {
            LiteRtRankedTensorType rankedTensorType;
            status = LiteRtGetRankedTensorType(inputTensor, &rankedTensorType);
            if (status != kLiteRtStatusOk)
            {
                spdlog::error("Failed to get ranked tensor type {}", static_cast<uint32_t>(status));
                return {};
            }

            spdlog::info("Found matching output tensor: {}", tensorName);
            spdlog::info("Ranked Tensor Type - Element Type: {}", static_cast<uint32_t>(rankedTensorType.element_type));
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
            return rankedTensorType;
        }
    }

    spdlog::error("No matching input tensor found for signature index {}", signatureIndex);
    return {};
}

LiteRtRankedTensorType LiteRtImageInference::_getOutputTensorType(LiteRtEnvironment environment,
                                                                  LiteRtCompiledModel compiledModel,
                                                                  LiteRtModel model,
                                                                  int signatureIndex,
                                                                  int outputIndex)
{
    LiteRtSubgraph subgraph;
    LiteRtStatus status = LiteRtGetModelSubgraph(m_model, signatureIndex, &subgraph);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get model subgraph {}", static_cast<uint32_t>(status));
        return {};
    }

    LiteRtParamIndex subgraphNumOutputs;
    LiteRtGetNumSubgraphOutputs(subgraph, &subgraphNumOutputs);

    std::vector<LiteRtTensor> outputs;
    for (auto i = 0; i < subgraphNumOutputs; ++i)
    {
        LiteRtTensor output;
        LiteRtGetSubgraphOutput(subgraph, i, &output);
        outputs.push_back(output);
    }

    LiteRtRankedTensorType rankedTensorType;
    status = LiteRtGetRankedTensorType(outputs[outputIndex], &rankedTensorType);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get ranked tensor type {}", static_cast<uint32_t>(status));
        return {};
    }

    return rankedTensorType;
}

LiteRtRankedTensorType LiteRtImageInference::_getOutputTensorType(LiteRtEnvironment environment,
                                                                  LiteRtCompiledModel compiledModel,
                                                                  LiteRtModel model,
                                                                  int signatureIndex,
                                                                  std::string_view outputName)
{
    LiteRtSubgraph subgraph;
    LiteRtStatus status = LiteRtGetModelSubgraph(m_model, signatureIndex, &subgraph);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get model subgraph {}", static_cast<uint32_t>(status));
        return {};
    }

    LiteRtParamIndex subgraphNumOutputs;
    status = LiteRtGetNumSubgraphOutputs(subgraph, &subgraphNumOutputs);
    if (status != kLiteRtStatusOk)
    {
        spdlog::error("Failed to get number of subgraph outputs {}", static_cast<uint32_t>(status));
        return {};
    }

    for (size_t i = 0; i < subgraphNumOutputs; ++i)
    {
        LiteRtTensor outputTensor;
        status = LiteRtGetSubgraphOutput(subgraph, i, &outputTensor);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get subgraph output {}", static_cast<uint32_t>(status));
            return {};
        }

        const char* tensorName;
        status = LiteRtGetTensorName(outputTensor, &tensorName);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor name {}", static_cast<uint32_t>(status));
            return {};
        }

        if (outputName == tensorName)
        {
            LiteRtRankedTensorType rankedTensorType;
            status = LiteRtGetRankedTensorType(outputTensor, &rankedTensorType);
            if (status != kLiteRtStatusOk)
            {
                spdlog::error("Failed to get ranked tensor type {}", static_cast<uint32_t>(status));
                return {};
            }

            spdlog::info("Found matching output tensor: {}", tensorName);
            spdlog::info("Ranked Tensor Type - Element Type: {}", static_cast<uint32_t>(rankedTensorType.element_type));
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

            return rankedTensorType;
        }
    }

    spdlog::error("No matching output tensor found for signature index {}", signatureIndex);
    return {};
}

std::vector<LiteRtTensorBuffer> LiteRtImageInference::_createGLInputTensorBuffer(LiteRtEnvironment environment,
                                                                                 LiteRtModel model,
                                                                                 LiteRtCompiledModel compiledModel,
                                                                                 int signatureIndex)
{
    std::vector<LiteRtTensorBuffer> inputTensorBuffers;

    LiteRtSignature signature = m_signatures[signatureIndex];
    LiteRtParamIndex numInputs;
    LiteRtGetNumSignatureInputs(signature, &numInputs);
    // Create input tensor buffers based on the model and signature information
    for (LiteRtParamIndex i = 0; i < numInputs; ++i)
    {
        LiteRtTensorBufferRequirements tensorBufferRequirements;
        LiteRtStatus status = LiteRtGetCompiledModelInputBufferRequirements(
            compiledModel, signatureIndex, i, &tensorBufferRequirements);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get input buffer requirements for signature index {}, input index {}, status: {}",
                          signatureIndex, i, static_cast<uint32_t>(status));
            return {};
        }

        LiteRtRankedTensorType rankedTensorType = _getInputTensorType(environment, compiledModel, model, signatureIndex, i);

        size_t tensorBufferSize;
        status = LiteRtGetTensorBufferRequirementsBufferSize(
            tensorBufferRequirements, &tensorBufferSize);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor buffer size {}", static_cast<uint32_t>(status));
            return {};
        }

        LiteRtTensorBuffer inputBuffer;
        status = LiteRtCreateManagedTensorBuffer(environment, kLiteRtTensorBufferTypeGlBuffer, &rankedTensorType, tensorBufferSize, &inputBuffer);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to create tensor buffer {}", static_cast<uint32_t>(status));
            return {};
        }

        inputTensorBuffers.push_back(inputBuffer);
    }

    return inputTensorBuffers;
}

std::vector<LiteRtTensorBuffer> LiteRtImageInference::_createGLOutputTensorBuffer(LiteRtEnvironment environment,
                                                                                  LiteRtModel model,
                                                                                  LiteRtCompiledModel compiledModel,
                                                                                  int signatureIndex)
{
    std::vector<LiteRtTensorBuffer> outputTensorBuffers;

    LiteRtSignature signature = m_signatures[signatureIndex];
    LiteRtParamIndex numOutputs;
    LiteRtGetNumSignatureOutputs(signature, &numOutputs);
    // Create output tensor buffers based on the model and signature information
    for (LiteRtParamIndex i = 0; i < numOutputs; ++i)
    {
        LiteRtTensorBufferRequirements tensorBufferRequirements;
        LiteRtStatus status = LiteRtGetCompiledModelOutputBufferRequirements(
            compiledModel, signatureIndex, i, &tensorBufferRequirements);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get output buffer requirements for signature index {}, output index {}, status: {}",
                          signatureIndex, i, static_cast<uint32_t>(status));
            return {};
        }

        LiteRtRankedTensorType rankedTensorType = _getInputTensorType(environment, compiledModel, model, signatureIndex, i);

        size_t tensorBufferSize;
        status = LiteRtGetTensorBufferRequirementsBufferSize(
            tensorBufferRequirements, &tensorBufferSize);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to get tensor buffer size {}", static_cast<uint32_t>(status));
            return {};
        }

        LiteRtTensorBuffer outputBuffer;
        status = LiteRtCreateManagedTensorBuffer(environment, kLiteRtTensorBufferTypeGlBuffer, &rankedTensorType, tensorBufferSize, &outputBuffer);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to create tensor buffer {}", static_cast<uint32_t>(status));
            return {};
        }

        outputTensorBuffers.push_back(outputBuffer);
    }

    return outputTensorBuffers;
}

std::vector<LiteRtSignature> LiteRtImageInference::_getSignatures(LiteRtEnvironment environment,
                                                                  LiteRtCompiledModel compiledModel,
                                                                  LiteRtModel model)
{
    LiteRtParamIndex numSignatures;
    LiteRtGetNumModelSignatures(model, &numSignatures);

    std::vector<LiteRtSignature> signatures{};
    signatures.reserve(numSignatures);
    for (LiteRtParamIndex i = 0; i < numSignatures; ++i)
    {
        LiteRtSignature liteRtSignature;
        LiteRtGetModelSignature(model, i, &liteRtSignature);

        signatures[i] = liteRtSignature;
    }

    return signatures;
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
    void* hostInputMemAddr;
    {
        auto status = LiteRtLockTensorBuffer(m_inputTensorBuffers[0], &hostInputMemAddr);
        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to lock tensor buffer for input index 0, status: {}", static_cast<uint32_t>(status));
            return {};
        }

        std::memcpy(hostInputMemAddr, m_preprocessed.data(), m_preprocessed.size() * sizeof(float));
        status = LiteRtUnlockTensorBuffer(m_inputTensorBuffers[0]);

        if (status != kLiteRtStatusOk)
        {
            spdlog::error("Failed to unlock tensor buffer for input index 0, status: {}", static_cast<uint32_t>(status));
            return {};
        }
    }

    auto status = LiteRtRunCompiledModel(
        m_compiledModel, /*signature_index=*/0,
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