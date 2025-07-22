#pragma once

#include "image.h"
#include "june/june.h"
#include "june/june_gles_service.h"
#include "litert_image_inference.h"

namespace jipu
{

class JuneGLESLiteRtService : public JuneGLESService
{

public:
    JuneGLESLiteRtService(const JuneServiceDescriptor& descriptor);
    ~JuneGLESLiteRtService();

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

    std::unique_ptr<LiteRtImageInference> m_liteRtInference{ nullptr };
};

} // namespace jipu