#include "june_gles_service1.h"

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

    //
    {

        GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        m_programObject = glCreateProgram();
        if (m_programObject == 0)
        {
            spdlog::debug("프로그램 객체 생성 실패");
        }
        glAttachShader(m_programObject, vertexShader);
        glAttachShader(m_programObject, fragmentShader);

        glBindAttribLocation(m_programObject, 0, "vPosition");
        glLinkProgram(m_programObject);

        GLint linked;
        glGetProgramiv(m_programObject, GL_LINK_STATUS, &linked);
        if (!linked)
        {
            GLint infoLen = 0;
            glGetProgramiv(m_programObject, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1)
            {
                char* infoLog = new char[infoLen];
                glGetProgramInfoLog(m_programObject, infoLen, nullptr, infoLog);
                spdlog::debug("프로그램 링크 에러: {}", infoLog);
                delete[] infoLog;
            }
            glDeleteProgram(m_programObject);
        }
    }

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
        JuneSharedMemory juneSharedMemory = m_juneAPI.InstanceCreateSharedMemory(m_juneInstance, &juneSharedMemoryDescriptor);
        setJuneSharedMemory("memory1", juneSharedMemory);
    }

    // Create Api Memory
    {
        JuneApiMemoryDescriptor juneApiMemoryDescriptor{};
        juneApiMemoryDescriptor.nextInChain = nullptr;
        juneApiMemoryDescriptor.sharedMemory = getJuneSharedMemory("memory1");

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

void JuneGLESService1::work()
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_eglImage);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        spdlog::debug("프레임버퍼 상태가 완전하지 않음");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        eglDestroyImageKHR(m_eglDisplay, m_eglImage);
    }

    glViewport(0, 0, m_descriptor.width, m_descriptor.height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_programObject);

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
        spdlog::debug("service1 is rendered in swapbuffer.");
        eglSwapBuffers(m_eglDisplay, m_eglSurface);
    }
    else
    {
        spdlog::debug("service1 is rendered in pbuffer.");
    }
}

} // namespace jipu
