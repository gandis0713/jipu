#pragma once

#include "jipu/common/dylib.h"
#include "june/june.h"
#include "june_api.h"
#include "june_service.h"

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

class JuneGLESService : public JuneService
{

public:
    JuneGLESService(const JuneServiceDescriptor& descriptor);
    ~JuneGLESService();

protected:
    void begin() override;
    void end() override;

protected:
    EGLContext m_eglContext = EGL_NO_CONTEXT;
    EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
    EGLConfig m_eglConfig = EGL_NO_CONFIG_KHR;
    EGLSurface m_eglSurface = EGL_NO_SURFACE;

    JuneInstance m_juneInstance{ nullptr };
    JuneApiContext m_juneApiContext{ nullptr };
};

} // namespace jipu