#pragma once

#include "june/june.h"
#include "june_gles_service.h"

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

private:
    GLuint m_programObject;
    JuneApiMemory m_juneApiMemory{ nullptr };
    EGLImageKHR m_eglImage{ nullptr };
};

} // namespace jipu