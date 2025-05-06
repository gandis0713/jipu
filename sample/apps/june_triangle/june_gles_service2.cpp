#include "june_gles_service2.h"

#include "file.h"
#include <spdlog/spdlog.h>

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

    {
        std::vector<char> vertex = utils::readFile(m_descriptor.appDir / "gles_service2_vert.glsl", m_descriptor.appHandle);
        std::vector<char> fragment = utils::readFile(m_descriptor.appDir / "gles_service2_frag.glsl", m_descriptor.appHandle);
        m_programObject2 = createProgram(vertex.data(), fragment.data());
        if (m_programObject2 == 0)
        {
            throw std::runtime_error("Failed to create program 2");
        }

        // 속성 위치 바인딩 (명시적으로 지정)
        glBindAttribLocation(m_programObject2, 0, "aPosition");
        glBindAttribLocation(m_programObject2, 1, "aTexCoord");

        glGenTextures(1, &m_texture);
        CHECK_GL_ERROR();
    }
    std::string label = "gles service2";

    createInstance(label);
    createApiContext(label);

    // Create Fence
    {
        JuneFenceCreateDescriptor fenceDescriptor;
        m_signalFence = m_juneAPI.ApiContextCreateFence(m_juneApiContext, &fenceDescriptor);
    }
}

void JuneGLESService2::work()
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

    spdlog::debug("gles service2 begin access");

    int count = 0;

    glBindTexture(GL_TEXTURE_2D, m_texture);

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

    // glFlush();
    // CHECK_GL_ERROR();
    // glFinish();
    // CHECK_GL_ERROR();

    if (m_descriptor.windowHandle)
    {
        spdlog::debug("gles service2 is rendered in swapbuffer.");
        eglSwapBuffers(m_eglDisplay, m_eglSurface);
        CHECK_GL_ERROR();
    }
    else
    {
        spdlog::debug("gles service2 is rendered in pbuffer.");
    }

    spdlog::debug("gles service2 end access");

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
    glDisableVertexAttribArray(texLoc);
    CHECK_GL_ERROR();
}

void JuneGLESService2::end()
{
    glDeleteTextures(1, &m_texture);
    CHECK_GL_ERROR();

    JuneGLESService::end();
}

void JuneGLESService2::setSharedMemory(JuneSharedMemory sharedMemory)
{
    std::lock_guard<std::mutex> lock(m_sharedMemoryMutex);

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
