#include "june_gles_service1.h"

#include <random>
#include <spdlog/spdlog.h>
#include <thread>

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
        m_programObject1 = createProgram(vertexShaderSource1, fragmentShaderSource1);
        if (m_programObject1 == 0)
        {
            spdlog::debug("Failed to create program");
            return;
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
        m_sharedMemory = m_juneAPI.InstanceCreateSharedMemory(m_juneInstance, &juneSharedMemoryDescriptor);
        m_sharingObjects.sharedMemory = m_sharedMemory;
    }

    // Create Fence
    {
        JuneFenceDescriptor fenceDescriptor;
        m_fence = m_juneAPI.ApiContextCreateFence(m_juneApiContext, &fenceDescriptor);

        m_sharingObjects.fences.push_back(m_fence);
    }

    // Create Resource
    {
        JuneResourceEGLImageCreateInfo eglImageCreateInfo;
        JuneResourceEGLImageResultInfo eglImageResultInfo;

        JuneResourceEGLImageDescriptor juneResourceEGLImageDescriptor{};
        juneResourceEGLImageDescriptor.chain.sType = JuneSType_EGLImageResourceDescriptor;
        juneResourceEGLImageDescriptor.eglImageCreateInfo = &eglImageCreateInfo;
        juneResourceEGLImageDescriptor.eglImageResultInfo = &eglImageResultInfo;

        JuneResourceDescriptor juneResourceDescriptor{};
        juneResourceDescriptor.nextInChain = &juneResourceEGLImageDescriptor.chain;
        juneResourceDescriptor.sharedMemory = m_sharedMemory;

        m_juneAPI.ApiContextCreateResource(m_juneApiContext, &juneResourceDescriptor);

        m_eglImage = eglImageResultInfo.eglImage;
        m_eglClientBuffer = eglImageResultInfo.eglClientBuffer;
    }
}

void JuneGLESService1::work()
{
    {
        std::lock_guard<std::mutex> lock(m_sharedMutex);
        if (m_shared)
        {
            JuneSharedMemoryExportedEGLSyncKHRSyncObject waitExportedEGLSyncKHRSyncObject{};
            {
                JuneSharedMemorySyncInfo waitSyncInfo{};
                waitSyncInfo.fences = m_sharedObjects.fences.data();
                waitSyncInfo.fenceCount = m_sharedObjects.fences.size();

                waitExportedEGLSyncKHRSyncObject.chain.sType = JuneSType_SharedMemoryExportedEGLSyncKHRSyncObject;

                JuneSharedMemoryExportedSyncObject exportedSyncObject{};
                exportedSyncObject.nextInChain = &waitExportedEGLSyncKHRSyncObject.chain;

                JuneApiContextBeginMemoryAccessDescriptor descriptor{};
                descriptor.sharedMemory = m_sharedMemory;
                descriptor.waitSyncInfo = &waitSyncInfo;
                descriptor.exportedSyncObject = &exportedSyncObject;

                m_juneAPI.ApiContextBeginMemoryAccess(m_juneApiContext, &descriptor);
            }

            EGLSyncKHR* eglSyncs = static_cast<EGLSyncKHR*>(waitExportedEGLSyncKHRSyncObject.eglSyncs);
            for (auto count = 0; count < waitExportedEGLSyncKHRSyncObject.eglSyncCount; ++count)
            {
                EGLSyncKHR* currentEGLSync = eglSyncs + count;
                EGLint eglResult = eglClientWaitSyncKHR(m_eglDisplay, *currentEGLSync, EGL_SIGNALED_KHR, 0);
                if (eglResult == EGL_FALSE)
                {
                    CHECK_GL_ERROR();
                    spdlog::error("gles service 1 eglClientWaitSyncKHR failed");
                    return;
                }
            }
        }
    }

    spdlog::debug("gles service1 begin access");

    int count = 0;
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
        spdlog::debug("Framebuffer is not complete");
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

    //    spdlog::debug("r: {}, g: {}, b: {}", r, g, b);

    GLfloat vertices[] = {
        -1.0f, 1.0f, 0.0f,  // 좌측 상단
        -1.0f, -1.0f, 0.0f, // 좌측 하단
        1.0f, 1.0f, 0.0f,   // 우측 상단
        1.0f, -1.0f, 0.0f   // 우측 하단
    };

    GLint posLoc = glGetAttribLocation(m_programObject1, "aPosition");
    CHECK_GL_ERROR();
    glEnableVertexAttribArray(posLoc);
    CHECK_GL_ERROR();
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    CHECK_GL_ERROR();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    CHECK_GL_ERROR();

    //        glFlush();
    //        CHECK_GL_ERROR();
    //        glFinish();
    //        CHECK_GL_ERROR();

    if (m_descriptor.windowHandle)
    {
        spdlog::debug("gles service1 is rendered in swapbuffer.");
        eglSwapBuffers(m_eglDisplay, m_eglSurface);
    }
    else
    {
        spdlog::debug("gles service1 is rendered in pbuffer.");
    }

    spdlog::debug("gles service1 end access");

    {
        std::lock_guard<std::mutex> lock(m_sharedMutex);
        if (m_shared)
        {
            JuneSharedMemoryExportedEGLSyncKHRSyncObject signalExportedEGLSyncKHRSyncObject{};
            {
                JuneSharedMemorySyncInfo signalSyncInfo{};
                signalSyncInfo.fences = m_sharingObjects.fences.data();
                signalSyncInfo.fenceCount = m_sharingObjects.fences.size();

                signalExportedEGLSyncKHRSyncObject.chain.sType = JuneSType_SharedMemoryExportedEGLSyncKHRSyncObject;

                JuneSharedMemoryExportedSyncObject exportedSyncObject{};
                exportedSyncObject.nextInChain = &signalExportedEGLSyncKHRSyncObject.chain;

                JuneApiContextEndMemoryAccessDescriptor descriptor{};
                descriptor.sharedMemory = m_sharedMemory;
                descriptor.signalSyncInfo = &signalSyncInfo;
                descriptor.exportedSyncObject = &exportedSyncObject;

                m_juneAPI.ApiContextEndMemoryAccess(m_juneApiContext, &descriptor);
            }

            for (auto count = 0; count < signalExportedEGLSyncKHRSyncObject.eglSyncCount; ++count)
            {
                EGLSyncKHR currentEGLSync = *(static_cast<EGLSyncKHR*>(signalExportedEGLSyncKHRSyncObject.eglSyncs) + count);
                // sharing currentEGLSync if needed.
            }
        }
    }

    glDisableVertexAttribArray(posLoc);
    CHECK_GL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CHECK_GL_ERROR();
    glDeleteFramebuffers(1, &fbo);
    CHECK_GL_ERROR();
    glDeleteTextures(1, &texture);
    CHECK_GL_ERROR();

    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

JuneServiceShareObjects JuneGLESService1::getSharingObject() const
{
    return m_sharingObjects;
}

void JuneGLESService1::setSharedObjects(const JuneServiceShareObjects& sharedObjects)
{
    m_sharedObjects = sharedObjects;

    std::lock_guard<std::mutex> lock(m_sharedMutex);
    m_shared = true;
}

} // namespace jipu
