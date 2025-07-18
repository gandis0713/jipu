#pragma once

#include "june/june.h"
#include "june/june_sample.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#endif

#include "tflite_inference.h"

namespace jipu
{

class JuneInferenceSample : public JuneSample
{
public:
    JuneInferenceSample() = delete;
    JuneInferenceSample(const JuneSampleDescriptor& descriptor);
    ~JuneInferenceSample() override;

    void init() override;

private:
    std::unique_ptr<TFLiteInference> m_tfliteInference{ nullptr };

    std::unique_ptr<JuneService> m_glesService1{ nullptr };

    [[maybe_unused]] bool m_glesService1Ready{ false };
};

} // namespace jipu