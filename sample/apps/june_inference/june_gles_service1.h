#pragma once

#include "june/june.h"
#include "june/june_gles_service.h"

#include "image.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
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

    void createEGLSurface() override;

private:
    GLuint m_programObject{ 0 };
    GLuint m_texture{};
    GLuint m_vbo{};
    GLuint m_vao{};
    GLint m_positionLoc{};
    GLint m_texCoordLoc{};
    GLint m_textureLoc{};

    std::unique_ptr<Image> m_image{ nullptr };
};

} // namespace jipu