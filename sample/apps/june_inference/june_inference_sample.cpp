#include "june_inference_sample.h"
#include "june_gles_service1.h"

#include "file.h"
#include "image.h"
#include <memory>
#include <spdlog/spdlog.h>

namespace jipu
{

JuneInferenceSample::JuneInferenceSample(const JuneSampleDescriptor& descriptor)
    : JuneSample(descriptor)
{
    std::string modelPath = m_sharingData.appDir / "deeplabv3.tflite";
    std::vector<char> modelBuffer = utils::readFile(modelPath, m_sharingData.appHandle);
    std::vector<char> imageBuffer = utils::readFile(m_sharingData.appDir / "man.png", m_sharingData.appHandle);

    m_tfliteInference = std::make_unique<TFLiteInference>();
    m_tfliteInference->setInputImage(std::make_unique<Image>(imageBuffer.data(), imageBuffer.size()));
    if (!m_tfliteInference->loadModel(modelBuffer))
    {
        spdlog::error("Failed to load model");
        return;
    }

    std::vector<uint8_t> result = m_tfliteInference->runInference();
    if (result.empty())
    {
        spdlog::error("Inference result is empty");
        return;
    }
}

JuneInferenceSample::~JuneInferenceSample()
{
}

void JuneInferenceSample::init()
{
    JuneSample::init();

    m_glesService1 = std::make_unique<JuneGLESService1>(JuneServiceDescriptor{
        .sharingData = &m_sharingData,
        .fps = 120,
    });
    m_glesService1->start(JuneServiceStartDescriptor{
        .callback = [this]() {
            m_glesService1Ready = true;
        } });
}

} // namespace jipu