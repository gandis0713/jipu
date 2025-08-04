#pragma once

#include "jipu/common/dylib.h"
#include "june/june.h"
#include "june_api.h"
#include "june_service.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <android/sync.h>
#endif

#define CHECK_EGL_ERROR(func_name)                                                                                \
    {                                                                                                             \
        EGLint err = eglGetError();                                                                               \
        if (err != EGL_SUCCESS)                                                                                   \
        {                                                                                                         \
            spdlog::error("EGL function '{}' failed with error code: {}", #func_name, static_cast<int32_t>(err)); \
        }                                                                                                         \
    }

#define CHECK_GL_ERROR(func_name)                                                                                 \
    {                                                                                                             \
        GLenum err = glGetError();                                                                                \
        if (err != GL_NO_ERROR)                                                                                   \
        {                                                                                                         \
            spdlog::error("GL function '{}' failed with error code: {}", #func_name, static_cast<uint32_t>(err)); \
        }                                                                                                         \
    }

namespace jipu
{

GLuint compileShader(GLenum type, const char* source);
GLuint createProgram(const char* vertexSource, const char* fragmentSource);
GLuint createTexture(unsigned char* imageData, int width, int height, int channels);

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

    PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR{ nullptr };
    PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR{ nullptr };
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES{ nullptr };
    PFNEGLDUPNATIVEFENCEFDANDROIDPROC eglDupNativeFenceFDANDROID{ nullptr };
    PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC eglGetNativeClientBufferANDROID{ nullptr };
    PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR{ nullptr };
    PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR{ nullptr };
    PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR{ nullptr };
    PFNEGLGETSYNCATTRIBKHRPROC eglGetSyncAttribKHR{ nullptr };
    PFNEGLWAITSYNCKHRPROC eglWaitSyncKHR{ nullptr };
};

} // namespace jipu