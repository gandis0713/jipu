#pragma once

#include "june/june.h"
#include "june/june_service.h"

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
// #include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <android/sync.h>
#endif

namespace jipu
{

class JuneService1 : public JuneService
{

public:
    JuneService1(const JuneServiceDescriptor& descriptor);
    ~JuneService1();

    void begin() override;
    void work() override;

    JuneServiceShareObjects getSharingObject() const override;
    void setSharedObjects(const JuneServiceShareObjects& sharedObjects) override;

private:
    JuneApiContext m_juneApiContext{ nullptr };
    JuneFence m_fence{ nullptr };

#if defined(__ANDROID__) || defined(ANDROID)
private:
    AHardwareBuffer* m_aHardwareBuffer{ nullptr };
#endif
};

} // namespace jipu