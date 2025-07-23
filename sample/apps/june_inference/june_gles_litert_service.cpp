#include "june_gles_litert_service.h"

#include "file.h"
#include "image.h"
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <random>
#include <spdlog/spdlog.h>
#include <thread>

namespace jipu
{

namespace
{

// 정점 셰이더 소스
const char* vertexShaderSource =
    "#version 100\n"
    "attribute vec4 a_position;\n"
    "attribute vec2 a_texCoord;\n"
    "varying vec2 v_texCoord;\n"
    "void main() {\n"
    "    gl_Position = a_position;\n"
    "    v_texCoord = a_texCoord;\n"
    "}\n";

// 프래그먼트 셰이더 소스
const char* fragmentShaderSource =
    // "#version 100\n"
    // "precision mediump float;\n"
    // "varying vec2 v_texCoord;\n"
    // "uniform sampler2D u_texture;\n"
    // "uniform sampler2D u_texture_mask;\n"
    // "void main() {\n"
    // "    vec3 color = texture2D(u_texture, v_texCoord).rgb;\n"
    // "    float mask = texture2D(u_texture_mask, v_texCoord).r;\n"
    // "    \n"
    // "    if (mask > 0.5) {\n"
    // "        gl_FragColor = vec4(color, 1.0);\n" // 완전 불투명
    // "    } else {\n"
    // "        discard;\n" // 픽셀 버리기 (완전히 제거)
    // "    }\n"
    // "}\n";
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "uniform sampler2D u_texture_mask;\n" // 마스크 텍스처 추가
    "void main() {\n"
    "    vec3 color = texture2D(u_texture, v_texCoord).rgb;\n"     // RGB 값만 추출
    "    float alpha = texture2D(u_texture_mask, v_texCoord).r;\n" // 마스크의 R 채널을 알파값으로 사용
    "    gl_FragColor = vec4(color, alpha);\n"                     // 최종 색상에 마스크를 알파값으로 적용
    "}\n";

void updateTexture(GLuint textureId, unsigned char* imageData, int width, int height, int channels)
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, imageData);
    CHECK_EGL_ERROR();
    CHECK_GL_ERROR();
    // glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace

JuneGLESLiteRtService::JuneGLESLiteRtService(const JuneServiceDescriptor& descriptor)
    : JuneGLESService(descriptor)
{
}

JuneGLESLiteRtService::~JuneGLESLiteRtService()
{
}

void JuneGLESLiteRtService::begin()
{
    JuneGLESService::begin();

    int width = 0;
    int height = 0;
    int inputChannels = 0;
    int outputChannels = 0;

    std::vector<uint8_t> result{};

    // Load TFLite model
    {
        m_liteRtInference = std::make_unique<LiteRtImageInference>();

        width = m_liteRtInference->getWidth();
        height = m_liteRtInference->getHeight();
        inputChannels = m_liteRtInference->getInputChannel();
        outputChannels = m_liteRtInference->getOutputChannel();

        spdlog::info("Model loaded with input size: {}x{}, channels: {}, output channels: {}", width, height, inputChannels, outputChannels);
    }

    // Load image
    {
        std::vector<char> imageBuffer = utils::readFile(m_descriptor.sharingData->appDir / "man.png", m_descriptor.sharingData->appHandle);
        if (imageBuffer.empty())
        {
            spdlog::error("Failed to read image file");
            return;
        }

        m_image = std::make_unique<Image>(imageBuffer.data(), imageBuffer.size(), width, height, inputChannels);
        m_texture = createTexture(m_image->getPixels(), m_image->getWidth(), m_image->getHeight(), m_image->getChannel());
    }

    // Create mask image
    {

        m_mask = std::make_unique<Image>();
        std::vector<unsigned char> maskPixels(width * height * outputChannels, 0); // Initialize with zeros
        m_mask->setPixels(maskPixels.data(), width, height, outputChannels);
    }

    {
        // set input image and inference
        {
            m_liteRtInference->setInputImage(m_image.get());

            // std::string modelPath = m_descriptor.sharingData->appDir / "deeplabv3.tflite";
            std::string modelPath = m_descriptor.sharingData->appDir / "mediapipe.tflite";
            std::vector<char> modelBuffer = utils::readFile(modelPath, m_descriptor.sharingData->appHandle);

            if (!m_liteRtInference->loadModel(modelBuffer))
            {
                spdlog::error("Failed to load model");
                return;
            }
            result = m_liteRtInference->runInference();
        }
    }

    // Update mask texture with inference result
    {
        if (result.empty())
        {
            spdlog::error("Inference result is empty");
            return;
        }

        m_mask->setPixels(result.data(), width, height, outputChannels);
        m_textureMask = createTexture(m_mask->getPixels(), m_mask->getWidth(), m_mask->getHeight(), m_mask->getChannel());
    }

    // Create and compile shaders, create program
    {
        m_programObject = createProgram(vertexShaderSource, fragmentShaderSource);

        if (m_programObject == 0)
        {
            throw std::runtime_error("Failed to create program 1");
        }
    }

    // Create vertex buffer object (VBO) and set up vertex attributes
    {
        // 정점 데이터 (위치 + 텍스처 좌표)
        float vertices[] = {
            // 위치 (x, y)    텍스처 좌표 (u, v)
            -1.0f, -1.0f, 0.0f, 1.0f, // 왼쪽 아래
            1.0f, -1.0f, 1.0f, 1.0f,  // 오른쪽 아래
            -1.0f, 1.0f, 0.0f, 0.0f,  // 왼쪽 위
            1.0f, 1.0f, 1.0f, 0.0f    // 오른쪽 위
        };

        // VBO 생성
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    }

    {
        m_positionLoc = glGetAttribLocation(m_programObject, "a_position");
        m_texCoordLoc = glGetAttribLocation(m_programObject, "a_texCoord");
        m_textureLoc = glGetUniformLocation(m_programObject, "u_texture");
        m_textureMaskLoc = glGetUniformLocation(m_programObject, "u_texture_mask");
    }

    // std::string label = "gles service1";

    // createInstance(label);
    // createApiContext(label);
}

void JuneGLESLiteRtService::work()
{
    // 화면 클리어
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 알파 블렌딩 활성화
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 프로그램 사용
    glUseProgram(m_programObject);

    // 버퍼 바인딩
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // 정점 어트리뷰트 설정
    glEnableVertexAttribArray(m_positionLoc);
    glVertexAttribPointer(m_positionLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(m_texCoordLoc);
    glVertexAttribPointer(m_texCoordLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // 메인 텍스처 바인딩 (텍스처 유닛 0)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(m_textureLoc, 0);

    // 마스크 텍스처 바인딩 (텍스처 유닛 1)
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textureMask);
    glUniform1i(m_textureMaskLoc, 1);

    // 삼각형 스트립으로 사각형 그리기
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 정리
    glDisableVertexAttribArray(m_positionLoc);
    glDisableVertexAttribArray(m_texCoordLoc);

    eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void JuneGLESLiteRtService::end()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteTextures(1, &m_texture);
    glDeleteTextures(1, &m_textureMask);
    glDeleteProgram(m_programObject);

    JuneGLESService::end();
}

void JuneGLESLiteRtService::createEGLSurface()
{
    if (!m_descriptor.sharingData->windowHandle)
    {
        spdlog::error("No window handle provided for EGL surface creation");
        return;
    }

#if defined(__ANDROID__) || defined(ANDROID)
    ANativeWindow* window = static_cast<ANativeWindow*>(m_descriptor.sharingData->windowHandle);
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, window, NULL);
    if (m_eglSurface == EGL_NO_SURFACE)
    {
        throw std::runtime_error("Failed to create EGL surface for anative window");
    }
#endif
}

} // namespace jipu
