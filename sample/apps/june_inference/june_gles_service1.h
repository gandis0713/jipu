#pragma once

#include "image.h"
#include "june/june.h"
#include "june/june_gles_service.h"
#include "tflite_image_inference.h"

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
    GLuint m_textureMask{};
    GLuint m_vbo{};
    GLint m_positionLoc{};
    GLint m_texCoordLoc{};
    GLint m_textureLoc{};
    GLint m_textureMaskLoc{};

    std::unique_ptr<Image> m_image{ nullptr };
    std::unique_ptr<Image> m_mask{ nullptr };

    std::unique_ptr<TFLiteImageInference> m_tfliteInference{ nullptr };
};

} // namespace jipu