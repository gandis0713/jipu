#include "june_gles_service1.h"

#include "file.h"
#include "image.h"
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
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "    gl_FragColor = texture2D(u_texture, v_texCoord);\n"
    "}\n";

} // namespace

JuneGLESService1::JuneGLESService1(const JuneServiceDescriptor& descriptor)
    : JuneGLESService(descriptor)
{
}

JuneGLESService1::~JuneGLESService1()
{
}

void JuneGLESService1::begin()
{
    JuneGLESService::begin();

    {
        m_programObject = createProgram(vertexShaderSource, fragmentShaderSource);

        if (m_programObject == 0)
        {
            throw std::runtime_error("Failed to create program 1");
        }
    }
    {
        std::vector<char> imageBuffer = utils::readFile(m_descriptor.sharingData->appDir / "man.png", m_descriptor.sharingData->appHandle);
        if (imageBuffer.empty())
        {
            spdlog::error("Failed to read image file");
            return;
        }
        m_image = std::make_unique<Image>(imageBuffer.data(), imageBuffer.size());
        m_texture = createTexture(static_cast<unsigned char*>(m_image->getPixels()), m_image->getWidth(), m_image->getHeight(), m_image->getChannel());
    }

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
    }

    // std::string label = "gles service1";

    // createInstance(label);
    // createApiContext(label);
}

void JuneGLESService1::work()
{
    // 화면 클리어
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 프로그램 사용
    glUseProgram(m_programObject);

    // 버퍼 바인딩
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // 정점 어트리뷰트 설정
    glEnableVertexAttribArray(m_positionLoc);
    glVertexAttribPointer(m_positionLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(m_texCoordLoc);
    glVertexAttribPointer(m_texCoordLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // 텍스처 바인딩
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(m_textureLoc, 0);

    // 삼각형 스트립으로 사각형 그리기
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 정리
    glDisableVertexAttribArray(m_positionLoc);
    glDisableVertexAttribArray(m_texCoordLoc);

    eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void JuneGLESService1::end()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteTextures(1, &m_texture);
    glDeleteProgram(m_programObject);

    JuneGLESService::end();
}

void JuneGLESService1::createEGLSurface()
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
