#include "june_gles_service2.h"

#include <spdlog/spdlog.h>

namespace jipu
{

namespace
{

const char* vertexShaderSource = R"(
    attribute vec4 vPosition;
    void main() {
    gl_Position = vPosition;
    }
)";

const char* fragmentShaderSource = R"(
    precision mediump float;
    void main() {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
)";

GLuint loadShader(GLenum type, const char* shaderSrc)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0)
    {
        spdlog::debug("Error: 셰이더 생성 실패");
        return 0;
    }

    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);

    // 컴파일 결과 확인
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
            spdlog::debug("셰이더 컴파일 에러: {}", infoLog);
            delete[] infoLog;
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

} // namespace

JuneGLESService2::JuneGLESService2(const JuneServiceDescriptor& descriptor)
    : JuneGLESService(descriptor)
{
}

JuneGLESService2::~JuneGLESService2()
{
}

void JuneGLESService2::begin()
{
    JuneGLESService::begin();

    // Create Shared Memory
    {
        JuneSharedMemoryDescriptor juneSharedMemoryDescriptor{};
#if defined(__ANDROID__) || defined(ANDROID)
        AHardwareBuffer_Desc ahbDesc = {
            .width = m_descriptor.width,
            .height = m_descriptor.height,
            .layers = 1,
            .format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM,
            .usage = AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT
        };

        JuneSharedMemoryAHardwareBufferDescriptor juneSharedMemoryAHardwareBufferDescriptor{};
        juneSharedMemoryAHardwareBufferDescriptor.chain.sType = JuneSType_AHardwareBufferSharedMemory;
        juneSharedMemoryAHardwareBufferDescriptor.aHardwareBuffer = nullptr;
        juneSharedMemoryAHardwareBufferDescriptor.aHardwareBufferDesc = &ahbDesc;

        juneSharedMemoryDescriptor.nextInChain = &juneSharedMemoryAHardwareBufferDescriptor.chain;
#endif
        m_juneSharedMemory = m_juneAPI.InstanceCreateSharedMemory(m_juneInstance, &juneSharedMemoryDescriptor);
    }

    // Create Api Memory
    {
        JuneApiMemoryDescriptor juneApiMemoryDescriptor{};
        juneApiMemoryDescriptor.nextInChain = nullptr;
        juneApiMemoryDescriptor.sharedMemory = m_juneSharedMemory;

        m_juneApiMemory = m_juneAPI.ApiContextCreateApiMemory(m_juneApiContext, &juneApiMemoryDescriptor);
    }

    // Create Resource
    {
        JuneResourceEGLImageDescriptor juneResourceEGLImageDescriptor{};
        juneResourceEGLImageDescriptor.chain.sType = JuneStype_EGLImageResourceDescriptor;
        JuneResourceDescriptor juneResourceDescriptor{};
        juneResourceDescriptor.nextInChain = &juneResourceEGLImageDescriptor.chain;

        m_eglImage = static_cast<EGLImageKHR>(m_juneAPI.ApiMemoryCreateResource(m_juneApiMemory, &juneResourceDescriptor));
    }
}

void JuneGLESService2::work()
{
    PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
    if (!eglDestroyImageKHR)
    {
        spdlog::debug("eglDestroyImageKHR 함수 포인터 획득 실패.");
    }

    // glEGLImageTargetTexture2DOES 함수 포인터 획득
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES =
        (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");
    if (!glEGLImageTargetTexture2DOES)
    {
        spdlog::debug("glEGLImageTargetTexture2DOES 함수 포인터 획득 실패.");
        eglDestroyImageKHR(m_eglDisplay, m_eglImage);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_eglImage);

    glViewport(0, 0, m_descriptor.width, m_descriptor.height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint programObject = glCreateProgram();
    if (programObject == 0)
    {
        spdlog::debug("프로그램 객체 생성 실패");
    }
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glBindAttribLocation(programObject, 0, "vPosition");
    glLinkProgram(programObject);

    GLint linked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = new char[infoLen];
            glGetProgramInfoLog(programObject, infoLen, nullptr, infoLog);
            spdlog::debug("프로그램 링크 에러: {}", infoLog);
            delete[] infoLog;
        }
        glDeleteProgram(programObject);
    }

    glUseProgram(programObject);

    GLfloat vertices[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    if (m_descriptor.windowHandle)
    {
        eglSwapBuffers(m_eglDisplay, m_eglSurface);
    }

    spdlog::debug("rendered.");
}

} // namespace jipu
