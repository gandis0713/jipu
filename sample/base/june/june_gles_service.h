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

#define CHECK_EGL_ERROR()                                                 \
    {                                                                     \
        EGLint err = eglGetError();                                       \
        if (err != EGL_SUCCESS)                                           \
        {                                                                 \
            spdlog::error("GL get error: {}", static_cast<int32_t>(err)); \
        }                                                                 \
    }

#define CHECK_GL_ERROR()                                                   \
    {                                                                      \
        GLenum err = glGetError();                                         \
        if (err != GL_NO_ERROR)                                            \
        {                                                                  \
            spdlog::error("GL get error: {}", static_cast<uint32_t>(err)); \
        }                                                                  \
    }

namespace jipu
{

GLuint compileShader(GLenum type, const char* source);
GLuint createProgram(const char* vertexSource, const char* fragmentSource);

class JuneGLESService : public JuneService
{

public:
    JuneGLESService(const JuneServiceDescriptor& descriptor);
    ~JuneGLESService();

protected:
    void begin() override;
    void end() override;

    void createApiContext(const std::string& label) override;
    virtual void createEGLSurface() {};

protected:
    EGLContext m_eglContext = EGL_NO_CONTEXT;
    EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
    EGLConfig m_eglConfig = EGL_NO_CONFIG_KHR;
    EGLSurface m_eglSurface = EGL_NO_SURFACE;

    PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR{ nullptr };
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES{ nullptr };
    PFNEGLDUPNATIVEFENCEFDANDROIDPROC eglDupNativeFenceFDANDROID{ nullptr };
    PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR{ nullptr };
    PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR{ nullptr };
    PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR{ nullptr };
    PFNEGLGETSYNCATTRIBKHRPROC eglGetSyncAttribKHR{ nullptr };
    PFNEGLWAITSYNCKHRPROC eglWaitSyncKHR{ nullptr };
};

} // namespace jipu