#pragma once

#include "june/june.h"
#include "june/june_sample.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#endif

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
    void createSharedMemories();

private:
#if defined(__ANDROID__) || defined(ANDROID)
    std::vector<AHardwareBuffer*> m_aHardwareBuffers{};
#endif
    std::vector<JuneSharedMemory> m_sharedMemories{};
};

} // namespace jipu