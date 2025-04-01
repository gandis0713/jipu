#include "june_gles_service3.h"

#include <random>
#include <spdlog/spdlog.h>

namespace jipu
{

namespace
{

#define CHECK_GL_ERROR()                                                   \
    {                                                                      \
        GLenum err = glGetError();                                         \
        if (err != GL_NO_ERROR)                                            \
        {                                                                  \
            spdlog::error("GL get error: {}", static_cast<uint32_t>(err)); \
        }                                                                  \
    }

const char* vertexShaderSource1 =
    "attribute vec4 aPosition;            \n"
    "void main() {                        \n"
    "    gl_Position = aPosition;         \n"
    "}                                    \n";
const char* fragmentShaderSource1 =
    "precision mediump float;             \n"
    "uniform vec4 uColor;                 \n"
    "void main() {                        \n"
    "    gl_FragColor = uColor;           \n"
    "}                                    \n";

const char* vertexShaderSource2 =
    "attribute vec4 aPosition;            \n"
    "attribute vec2 aTexCoord;            \n"
    "varying vec2 vTexCoord;              \n"
    "void main() {                      \n"
    "    gl_Position = aPosition;         \n"
    "    vTexCoord = aTexCoord;           \n"
    "}                                    \n";
const char* fragmentShaderSource2 =
    "precision mediump float;             \n"
    "varying vec2 vTexCoord;              \n"
    "uniform sampler2D uTexture;          \n"
    "void main() {                        \n"
    "    gl_FragColor = texture2D(uTexture, vTexCoord); \n"
    "}                                    \n";

GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = (char*)malloc(infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            // 로그 출력 (실제 환경에서는 로그 출력 함수 사용)
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader)
        return 0;

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader)
        return 0;

    GLuint program = glCreateProgram();
    if (program == 0)
        return 0;

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // 속성 위치 바인딩 (명시적으로 지정)
    glBindAttribLocation(program, 0, "aPosition");
    glBindAttribLocation(program, 1, "aTexCoord");

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = (char*)malloc(infoLen);
            glGetProgramInfoLog(program, infoLen, NULL, infoLog);
            spdlog::error("Failed to link program: {}", infoLog);
            free(infoLog);
        }
        glDeleteProgram(program);
        return 0;
    }

    // 쉐이더 객체는 프로그램에 첨부 후 삭제 가능
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

} // namespace

JuneGLESService3::JuneGLESService3(const JuneServiceDescriptor& descriptor)
    : JuneGLESService(descriptor)
{
}

JuneGLESService3::~JuneGLESService3()
{
}

void JuneGLESService3::begin()
{
    JuneGLESService::begin();

    // initialize program1 for EGL
    {
        m_programObject1 = createProgram(vertexShaderSource1, fragmentShaderSource1);
        if (m_programObject1 == 0)
        {
            spdlog::debug("프로그램1 객체 생성 실패");
            return;
        }
    }

    // initialize program2
    {
        m_programObject2 = createProgram(vertexShaderSource2, fragmentShaderSource2);
        if (m_programObject2 == 0)
        {
            spdlog::debug("프로그램2 객체 생성 실패");
            return;
        }
    }

    // Create Shared Memory
    JuneSharedMemory sharedMemory{};
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
        sharedMemory = m_juneAPI.InstanceCreateSharedMemory(m_juneInstance, &juneSharedMemoryDescriptor);
        m_sharingObjects.sharedMemory = sharedMemory;
    }

    // Create Api Memory
    JuneApiMemory apiMemory{};
    {
        JuneApiMemoryDescriptor juneApiMemoryDescriptor{};
        juneApiMemoryDescriptor.nextInChain = nullptr;
        juneApiMemoryDescriptor.sharedMemory = m_sharingObjects.sharedMemory;

        apiMemory = m_juneAPI.ApiContextCreateApiMemory(m_juneApiContext, &juneApiMemoryDescriptor);
        m_sharingObjects.apiMemories.push_back(apiMemory);
    }

    // Create Resource
    {
        JuneResourceEGLImageDescriptor juneResourceEGLImageDescriptor{};
        juneResourceEGLImageDescriptor.chain.sType = JuneStype_EGLImageResourceDescriptor;
        JuneResourceDescriptor juneResourceDescriptor{};
        juneResourceDescriptor.nextInChain = &juneResourceEGLImageDescriptor.chain;

        m_eglImage = static_cast<EGLImageKHR>(m_juneAPI.ApiMemoryCreateResource(apiMemory, &juneResourceDescriptor));
    }
}

void JuneGLESService3::work()
{
    {
        GLuint texture;
        glGenTextures(1, &texture);
        CHECK_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, texture);
        CHECK_GL_ERROR();
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_eglImage);
        CHECK_GL_ERROR();

        // 렌더링을 위해 FBO 생성 및 텍스처 부착
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        CHECK_GL_ERROR();
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        CHECK_GL_ERROR();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        CHECK_GL_ERROR();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            spdlog::debug("프레임버퍼 상태가 완전하지 않음");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &fbo);
            return;
        }

        glViewport(0, 0, m_descriptor.width, m_descriptor.height);
        CHECK_GL_ERROR();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        CHECK_GL_ERROR();
        glClear(GL_COLOR_BUFFER_BIT);
        CHECK_GL_ERROR();

        glUseProgram(m_programObject1);
        CHECK_GL_ERROR();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        float r = dis(gen);
        float g = dis(gen);
        float b = dis(gen);

        GLint colorLoc = glGetUniformLocation(m_programObject1, "uColor");
        glUniform4f(colorLoc, r, g, b, 1.0f);

        spdlog::debug("r: {}, g: {}, b: {}", r, g, b);

        GLfloat vertices[] = {
            0.0f, 0.8f, 0.0f,
            -0.8f, -0.8f, 0.0f,
            0.8f, -0.8f, 0.0f
        };
        GLint posLoc = glGetAttribLocation(m_programObject1, "aPosition");
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(posLoc);
        CHECK_GL_ERROR();
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
        CHECK_GL_ERROR();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        CHECK_GL_ERROR();

        //        glFlush();
        //        CHECK_GL_ERROR();
        //        glFinish();
        //        CHECK_GL_ERROR();

        glDisableVertexAttribArray(posLoc);
        CHECK_GL_ERROR();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        CHECK_GL_ERROR();
        glDeleteFramebuffers(1, &fbo);
        CHECK_GL_ERROR();
        glDeleteTextures(1, &texture);
        CHECK_GL_ERROR();
    }

    {
        GLuint texture;
        CHECK_GL_ERROR();
        glGenTextures(1, &texture);
        CHECK_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, texture);
        CHECK_GL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECK_GL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECK_GL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        CHECK_GL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECK_GL_ERROR();
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_eglImage);
        CHECK_GL_ERROR();

        glUseProgram(m_programObject2);
        CHECK_GL_ERROR();

        // full-screen quad 정점 및 텍스처 좌표
        GLfloat vertices[] = {
            -1.0f, 1.0f, 0.0f,  // 좌측 상단
            -1.0f, -1.0f, 0.0f, // 좌측 하단
            1.0f, -1.0f, 0.0f,  // 우측 하단
            1.0f, 1.0f, 0.0f    // 우측 상단
        };
        GLfloat texCoords[] = {
            0.0f, 0.0f, // 좌측 상단
            0.0f, 1.0f, // 좌측 하단
            1.0f, 1.0f, // 우측 하단
            1.0f, 0.0f  // 우측 상단
        };
        GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

        GLint posLoc = glGetAttribLocation(m_programObject2, "aPosition");
        CHECK_GL_ERROR();
        GLint texLoc = glGetAttribLocation(m_programObject2, "aTexCoord");
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(posLoc);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(texLoc);
        CHECK_GL_ERROR();
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
        CHECK_GL_ERROR();
        glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
        CHECK_GL_ERROR();

        // 텍스처 유니폼 설정
        GLint texUniform = glGetUniformLocation(m_programObject2, "uTexture");
        CHECK_GL_ERROR();
        glUniform1i(texUniform, 0);
        CHECK_GL_ERROR();

        glViewport(0, 0, m_descriptor.width, m_descriptor.height);
        CHECK_GL_ERROR();
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        CHECK_GL_ERROR();
        glClear(GL_COLOR_BUFFER_BIT);
        CHECK_GL_ERROR();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
        CHECK_GL_ERROR();

        //        glFlush();
        //        CHECK_GL_ERROR();
        //        glFinish();
        //        CHECK_GL_ERROR();

        if (m_descriptor.windowHandle)
        {
            spdlog::debug("service3 is rendered in swapbuffer.");
            eglSwapBuffers(m_eglDisplay, m_eglSurface);
            CHECK_GL_ERROR();
        }
        else
        {
            spdlog::debug("service3 is rendered in pbuffer.");
        }

        // 리소스 정리
        glDisableVertexAttribArray(posLoc);
        CHECK_GL_ERROR();
        glDisableVertexAttribArray(texLoc);
        CHECK_GL_ERROR();
        glDeleteTextures(1, &texture);
        CHECK_GL_ERROR();
    }
}

JuneServiceShareObjects JuneGLESService3::getSharingObject() const
{
    return {};
}

void JuneGLESService3::setSharedObjects(const JuneServiceShareObjects& sharedObjects)
{
}

} // namespace jipu
