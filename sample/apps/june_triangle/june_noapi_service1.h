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

private:
#if defined(__ANDROID__) || defined(ANDROID)
    AHardwareBuffer* m_aHardwareBuffer1{ nullptr };
    AHardwareBuffer* m_aHardwareBuffer2{ nullptr };
    AHardwareBuffer_Desc m_aHardwareBufferDesc1{};
    AHardwareBuffer_Desc m_aHardwareBufferDesc2{};
    uint32_t m_bytesPerPixel1{ 0 };
    uint32_t m_bytesPerPixel2{ 0 };
#endif
    int32_t m_signalFD{ -1 };
};

} // namespace jipu