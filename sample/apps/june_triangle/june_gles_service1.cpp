#include "june_gles_service1.h"

#include <random>
#include <spdlog/spdlog.h>
#include <thread>

#include "file.h"

namespace jipu
{

namespace
{

GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0)
    {
        spdlog::error("Failed to create shader");
        return shader;
    }
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
    CHECK_GL_ERROR();
    if (!vertexShader)
        return 0;

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    CHECK_GL_ERROR();
    if (!fragmentShader)
        return 0;

    GLuint program = glCreateProgram();
    CHECK_GL_ERROR();
    if (program == 0)
        return 0;

    glAttachShader(program, vertexShader);
    CHECK_GL_ERROR();
    glAttachShader(program, fragmentShader);
    CHECK_GL_ERROR();

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

    {
        std::vector<char> vertex = utils::readFile(m_descriptor.appDir / "gles_service1_vert.glsl", m_descriptor.appHandle);
        std::vector<char> fragment = utils::readFile(m_descriptor.appDir / "gles_service1_frag.glsl", m_descriptor.appHandle);
        m_programObject1 = createProgram(vertex.data(), fragment.data());
        if (m_programObject1 == 0)
        {
            throw std::runtime_error("Failed to create program 1");
        }

        glBindAttribLocation(m_programObject1, 0, "aPosition");
    }

    std::string label = "gles service1";

    createInstance(label);
    createApiContext(label);

    // Create Fence
    {
        JuneFenceCreateDescriptor fenceDescriptor;
        m_signalFence = m_juneAPI.ApiContextCreateFence(m_juneApiContext, &fenceDescriptor);
    }
}

void JuneGLESService1::work()
{
    if (!getSharedMemory())
        return;

    {
        std::vector<EGLSyncKHR> waitEGLSyncs{};
        std::vector<JuneFence> waitFences = getWaitFences();

        for (const auto& fence : waitFences)
        {
            JuneFenceEGLSyncExportDescriptor eglSyncExportDescriptor{};
            eglSyncExportDescriptor.chain.sType = JuneSType_FenceEGLSyncExportDescriptor;

            JuneFenceExportDescriptor descriptor{};
            descriptor.nextInChain = &eglSyncExportDescriptor.chain;

            m_juneAPI.FenceExport(fence, &descriptor);

            if (eglSyncExportDescriptor.eglSync)
            {
                waitEGLSyncs.push_back(eglSyncExportDescriptor.eglSync);
            }
        }

        for (auto count = 0; count < waitEGLSyncs.size(); ++count)
        {
            if (waitEGLSyncs[count] == nullptr)
                continue;

            // EGLint eglResult = eglWaitSyncKHR(m_eglDisplay, waitEGLSyncs[count], EGL_SIGNALED_KHR);
            // if (eglResult == EGL_FALSE)
            // {
            //     CHECK_GL_ERROR();
            //     spdlog::error("gles service 1 eglWaitSyncKHR failed");
            //     return;
            // }

            EGLint eglResult = eglClientWaitSyncKHR(m_eglDisplay, waitEGLSyncs[count], EGL_SIGNALED_KHR, 0);
            if (eglResult == EGL_FALSE)
            {
                CHECK_GL_ERROR();
                spdlog::error("gles service 1 eglClientWaitSyncKHR failed");
                return;
            }
        }
    }

    spdlog::debug("gles service1 begin access");

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
        JuneFenceResetDescriptor descriptor{};
        m_juneAPI.FenceReset(m_signalFence, &descriptor);

        {
            JuneFenceEGLSyncExportDescriptor eglSyncExportDescriptor{};
            eglSyncExportDescriptor.chain.sType = JuneSType_FenceEGLSyncExportDescriptor;

            JuneFenceExportDescriptor exportDescriptor{};
            exportDescriptor.nextInChain = &eglSyncExportDescriptor.chain;
            m_juneAPI.FenceExport(m_signalFence, &exportDescriptor);

            // EGLSyncKHR eglSync = eglSyncExportDescriptor.eglSync;
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

void JuneGLESService1::setSharedMemory(JuneSharedMemory sharedMemory)
{
    m_sharedMemory = sharedMemory;

    // Create Resource
    {
        JuneResourceEGLImageCreateInfo eglImageCreateInfo;
        JuneResourceEGLImageResultInfo eglImageResultInfo;

        JuneResourceEGLImageCreateDescriptor juneResourceEGLImageDescriptor{};
        juneResourceEGLImageDescriptor.chain.sType = JuneSType_ResourceEGLImageCreateDescriptor;
        juneResourceEGLImageDescriptor.eglImageCreateInfo = &eglImageCreateInfo;
        juneResourceEGLImageDescriptor.eglImageResultInfo = &eglImageResultInfo;

        JuneResourceCreateDescriptor juneResourceDescriptor{};
        juneResourceDescriptor.nextInChain = &juneResourceEGLImageDescriptor.chain;
        juneResourceDescriptor.sharedMemory = m_sharedMemory;

        m_juneAPI.ApiContextCreateResource(m_juneApiContext, &juneResourceDescriptor);

        m_eglImage = eglImageResultInfo.eglImage;
        m_eglClientBuffer = eglImageResultInfo.eglClientBuffer;
    }
}

} // namespace jipu
