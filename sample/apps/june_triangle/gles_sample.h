#pragma once

#include "jipu/common/dylib.h"
#include "june/june.h"
#include "june_api.h"
#include "runner.h"

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

struct GLESSampleDescriptor
{
    uint32_t fps{ 0 };
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    void* windowHandle{ nullptr };
};

class GLESSample
{

public:
    GLESSample(const GLESSampleDescriptor& descriptor);
    ~GLESSample();

    void run();
    void pause();
    void resume();
    void stop();
    bool isRunning() const;

private:
    const GLESSampleDescriptor m_descriptor;
    Runner m_runner;

    EGLContext m_eglContext = EGL_NO_CONTEXT;
    EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
    EGLConfig m_eglConfig = EGL_NO_CONFIG_KHR;
    EGLSurface m_eglSurface = EGL_NO_SURFACE;

    DyLib m_juneLib;
    JuneAPI m_juneAPI;

    JuneInstance m_juneInstance{ nullptr };
    JuneApiContext m_juneApiContext{ nullptr };
    JuneSharedMemory m_juneSharedMemory{ nullptr };
    JuneApiMemory m_juneApiMemory{ nullptr };

    EGLImageKHR m_eglImage{ nullptr };
};

} // namespace jipu