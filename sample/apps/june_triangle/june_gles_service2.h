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

class JuneGLESService2 : public JuneGLESService
{

public:
    JuneGLESService2(const JuneServiceDescriptor& descriptor);
    ~JuneGLESService2();

    void begin() override;
    void work() override;
    void end() override;

    JuneServiceShareObjects getSharingObject() const override;
    void setSharedObjects(const JuneServiceShareObjects& sharedObjects) override;

private:
    GLuint m_programObject2{ 0 };
    GLuint m_texture{ 0 };
    JuneResource m_juneResource{ nullptr };
    EGLImageKHR m_eglImage{ nullptr };

    JuneServiceShareObjects m_sharedObjects{};
};

} // namespace jipu