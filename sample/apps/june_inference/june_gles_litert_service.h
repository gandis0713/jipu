#pragma once

#include "android_camera.h"
#include "image.h"
#include "june/june.h"
#include "june/june_gles_service.h"
#include "litert_image_inference.h"

#include <mutex>

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

    std::mutex m_frameMutex;
    AImage* m_currentImage{ nullptr };
    AHardwareBuffer* m_currentHardwareBuffer{ nullptr };
    std::vector<uint8_t> m_frameData{};
    std::unordered_map<AHardwareBuffer*, EGLImageKHR> m_frames{};
    AndroidCamera m_androidCamera;
};

} // namespace jipu