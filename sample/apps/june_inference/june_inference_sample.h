#pragma once

#include "june/june.h"
#include "june/june_sample.h"

#include <android/hardware_buffer.h>
#include <android/native_window.h>

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
    std::unique_ptr<JuneService> m_glesTFLiteService{ nullptr };
    std::unique_ptr<JuneService> m_glesLiteRtService{ nullptr };

    [[maybe_unused]] bool m_glesTFLiteServiceReady{ false };
    [[maybe_unused]] bool m_glesLiteRtServiceReady{ false };
};

} // namespace jipu