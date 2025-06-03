#pragma once

#include "june/june.h"
#include "june/june_gles_service.h"

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

class JuneGLESService1 : public JuneGLESService
{

public:
    JuneGLESService1(const JuneServiceDescriptor& descriptor);
    ~JuneGLESService1();

    void begin() override;
    void work() override;
    void end() override;

    void addSharedMemory(JuneSharedMemory sharedMemory) override;
    void createEGLSurface() override;

private:
    GLuint m_programObject{ 0 };

    std::vector<GLuint> m_textures;
    std::vector<EGLImageKHR> m_eglImages;
    std::vector<EGLClientBuffer> m_eglClientBuffers;
    EGLSyncKHR m_eglSync{ EGL_NO_SYNC_KHR };
};

} // namespace jipu