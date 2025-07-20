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