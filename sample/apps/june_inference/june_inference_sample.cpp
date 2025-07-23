#include "june_inference_sample.h"
#include "june_gles_litert_service.h"
#include "june_gles_tflite_service.h"

#include "file.h"
#include "image.h"
#include <memory>
#include <spdlog/spdlog.h>

namespace jipu
{

JuneInferenceSample::JuneInferenceSample(const JuneSampleDescriptor& descriptor)
    : JuneSample(descriptor)
{
}

JuneInferenceSample::~JuneInferenceSample()
{
}

void JuneInferenceSample::init()
{
    JuneSample::init();

    if (m_runWithLiteRt)
    {
        m_glesLiteRtService = std::make_unique<JuneGLESLiteRtService>(JuneServiceDescriptor{
            .sharingData = &m_sharingData,
            .fps = 120,
        });
        m_glesLiteRtService->start(JuneServiceStartDescriptor{
            .callback = [this]() {
                m_glesLiteRtServiceReady = true;
            } });
    }
    else
    {
        m_glesTFLiteService = std::make_unique<JuneGLESTFLiteService>(JuneServiceDescriptor{
            .sharingData = &m_sharingData,
            .fps = 120,
        });
        m_glesTFLiteService->start(JuneServiceStartDescriptor{
            .callback = [this]() {
                m_glesTFLiteServiceReady = true;
            } });
    }
}

} // namespace jipu