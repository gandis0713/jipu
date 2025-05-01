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
    if(shader == 0)
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
    }

    // Create Instance
    {
        JuneInstanceDescriptor juneInstanceDescriptor{};
        m_juneInstance = m_juneAPI.CreateInstance(&juneInstanceDescriptor);
    }

    // Create Api Context
    {
        JuneGLESApiContextDescriptor juenGLESApiContextDescriptor{};
        juenGLESApiContextDescriptor.chain.sType = JuneSType_GLESApiContext;
        juenGLESApiContextDescriptor.display = m_eglDisplay;
        juenGLESApiContextDescriptor.context = m_eglContext;

        std::string label = "[GLESService2]";
        JuneApiContextDescriptor juneApiContextDescriptor;
        juneApiContextDescriptor.nextInChain = &juenGLESApiContextDescriptor.chain;
        juneApiContextDescriptor.label.data = label.data();
        juneApiContextDescriptor.label.length = label.length();
        m_juneApiContext = m_juneAPI.InstanceCreateApiContext(m_juneInstance, &juneApiContextDescriptor);
    }

    // Create Fence
    {
        JuneFenceDescriptor fenceDescriptor;
        m_fence = m_juneAPI.ApiContextCreateFence(m_juneApiContext, &fenceDescriptor);

        m_sharingObjects.fences.push_back(m_fence);
    }

    glGenTextures(1, &m_texture);
    CHECK_GL_ERROR();
}

void JuneGLESService2::work()
{
    {
        std::lock_guard<std::mutex> lock(m_sharedMutex);
        if (!m_shared)
        {
            spdlog::trace("GLES service2 is not shared.");
            return;
        }
    }

    JuneSharedMemoryExportedEGLSyncKHRSyncObject waitExportedEGLSyncKHRSyncObject{};
    {
        JuneSharedMemorySyncInfo waitSyncInfo{};
        waitSyncInfo.fences = m_sharedObjects.fences.data();
        waitSyncInfo.fenceCount = m_sharedObjects.fences.size();

        waitExportedEGLSyncKHRSyncObject.chain.sType = JuneSType_SharedMemoryExportedEGLSyncKHRSyncObject;

        JuneSharedMemoryExportedSyncObject exportedSyncObject{};
        exportedSyncObject.nextInChain = &waitExportedEGLSyncKHRSyncObject.chain;

        JuneApiContextBeginMemoryAccessDescriptor descriptor{};
        descriptor.sharedMemory = m_sharedObjects.sharedMemory;
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
            spdlog::error("gles service2 eglClientWaitSyncKHR failed");
            return;
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
    JuneSharedMemoryExportedEGLSyncKHRSyncObject signalExportedEGLSyncKHRSyncObject{};
    {
        JuneSharedMemorySyncInfo signalSyncInfo{};
        signalSyncInfo.fences = m_sharingObjects.fences.data();
        signalSyncInfo.fenceCount = m_sharingObjects.fences.size();

        signalExportedEGLSyncKHRSyncObject.chain.sType = JuneSType_SharedMemoryExportedEGLSyncKHRSyncObject;

        JuneSharedMemoryExportedSyncObject exportedSyncObject{};
        exportedSyncObject.nextInChain = &signalExportedEGLSyncKHRSyncObject.chain;

        JuneApiContextEndMemoryAccessDescriptor descriptor{};
        descriptor.sharedMemory = m_sharedObjects.sharedMemory;
        descriptor.signalSyncInfo = &signalSyncInfo;
        descriptor.exportedSyncObject = &exportedSyncObject;

        m_juneAPI.ApiContextEndMemoryAccess(m_juneApiContext, &descriptor);
    }

    for (auto count = 0; count < signalExportedEGLSyncKHRSyncObject.eglSyncCount; ++count)
    {
        EGLSyncKHR currentEGLSync = *(static_cast<EGLSyncKHR*>(signalExportedEGLSyncKHRSyncObject.eglSyncs) + count);
        // sharing currentEGLSync if needed.
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

JuneServiceShareObjects JuneGLESService2::getSharingObject() const
{
    return m_sharingObjects;
}

void JuneGLESService2::setSharedObjects(const JuneServiceShareObjects& sharedObjects)
{
    m_sharedObjects = sharedObjects;

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
        juneResourceDescriptor.sharedMemory = m_sharedObjects.sharedMemory;

        m_juneAPI.ApiContextCreateResource(m_juneApiContext, &juneResourceDescriptor);

        m_eglImage = eglImageResultInfo.eglImage;
        m_eglClientBuffer = eglImageResultInfo.eglClientBuffer;
    }

    std::lock_guard<std::mutex> lock(m_sharedMutex);
    m_shared = true;
}

} // namespace jipu
