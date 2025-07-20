#pragma once

#include "june/june.h"
#include "june/june_noapi_service.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <android/sync.h>
#endif

namespace jipu
{

class JuneNoApiService1 : public JuneNoApiService
{

public:
    JuneNoApiService1(const JuneServiceDescriptor& descriptor);
    ~JuneNoApiService1();

    void begin() override;
    void work() override;

#if defined(__ANDROID__) || defined(ANDROID)
    void addAHardwareBuffer(AHardwareBuffer* aHardwareBuffer);
#endif

private:
#if defined(__ANDROID__) || defined(ANDROID)
    std::vector<AHardwareBuffer*> m_aHardwareBuffers;
    std::vector<AHardwareBuffer_Desc> m_aHardwareBufferDescs;
    std::vector<uint32_t> m_bytesPerPixels;
    int32_t m_signalFD{ -1 };
#endif
};

} // namespace jipu