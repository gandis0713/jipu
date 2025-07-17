#include "june_inference_sample.h"

#include "file.h"
#include <memory>
#include <spdlog/spdlog.h>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"

#include "tflite/c/c_api.h"

namespace jipu
{

JuneInferenceSample::JuneInferenceSample(const JuneSampleDescriptor& descriptor)
    : JuneSample(descriptor)
{
    std::vector<char> buffer = utils::readFile(m_sharingData.appDir / "deeplabv3.tflite", m_sharingData.appHandle);
    const char* version = TfLiteVersion();
    spdlog::info("charles TensorFlow Lite version: {}", version);
    // std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromBuffer(buffer.data(), buffer.size());
}

JuneInferenceSample::~JuneInferenceSample()
{
}

void JuneInferenceSample::init()
{
    JuneSample::init();

    loadJuneLibrary();
    createInstance("JuneInferenceSample");
    createSharedMemories();

    if (m_sharedMemories.empty())
    {
        spdlog::error("No shared memories created, exiting.");
        return;
    }
}

void JuneInferenceSample::createSharedMemories()
{
    for (size_t i = 0; i < 3; ++i)
    {
        AHardwareBuffer* aHardwareBuffer = nullptr;

        AHardwareBuffer_Desc aHardwareBufferDesc = {
            .width = m_width,
            .height = m_height,
            .layers = 1,
            .format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM,
            .usage = AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT | AHARDWAREBUFFER_USAGE_CPU_WRITE_RARELY
        };

        int result = AHardwareBuffer_allocate(&aHardwareBufferDesc, &aHardwareBuffer);
        if (result != 0)
        {
            spdlog::error("Failed to allocate AHardwareBuffer: {}", result);
            return;
        }

        m_aHardwareBuffers.push_back(aHardwareBuffer);

        JuneSharedMemoryAHardwareBufferImportDescriptor aHardwareBufferImportDescriptor{
            .chain = { .sType = JuneSType_SharedMemoryAHardwareBufferImportDescriptor },
            .aHardwareBuffer = aHardwareBuffer
        };

        JuneSharedMemoryImportDescriptor juneSharedMemoryImportDescriptor{
            .nextInChain = &aHardwareBufferImportDescriptor.chain,
            .label = { .data = "JuneInferenceSampleSharedMemoryAHardwareBuffer", .length = 32 }
        };

        m_sharedMemories.push_back(m_juneAPI.InstanceImportSharedMemory(m_juneInstance, &juneSharedMemoryImportDescriptor));
    }
}

} // namespace jipu