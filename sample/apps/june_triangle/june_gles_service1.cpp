#include "june_gles_service1.h"

#include "file.h"
#include <random>
#include <spdlog/spdlog.h>
#include <thread>

namespace jipu
{

namespace
{

const char* vertexShaderSource = R"(#version 310 es
in vec4 aPosition;

void main()
{
    gl_Position = aPosition;
}
)";

const char* fragmentShaderSource = R"(#version 310 es
precision mediump float;
uniform vec4 uColor;
out vec4 fragColor;

void main()
{
    fragColor = uColor;
}
)";

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
        // {
        //     std::vector<char> vertex = utils::readFile(m_descriptor.appDir / "gles_service1_vert.glsl", m_descriptor.appHandle);
        //     std::vector<char> fragment = utils::readFile(m_descriptor.appDir / "gles_service1_frag.glsl", m_descriptor.appHandle);
        //     m_programObject = createProgram(vertex.data(), fragment.data());
        // }
        {
            m_programObject = createProgram(vertexShaderSource, fragmentShaderSource);
        }
        if (m_programObject == 0)
        {
            throw std::runtime_error("Failed to create program 1");
        }

        glBindAttribLocation(m_programObject, 0, "aPosition");

        GLuint texture1;
        glGenTextures(1, &texture1);
        m_textures.push_back(texture1);
    }
    std::string label = "gles service1";

    createInstance(label);
    createApiContext(label);

    // Create Fence
    {
        std::string label = "gles service1 fence";
        JuneFenceCreateDescriptor fenceDescriptor;
        fenceDescriptor.label.data = label.c_str();
        fenceDescriptor.label.length = static_cast<uint32_t>(label.length());
        fenceDescriptor.type = JuneFenceType_SyncFD;
        m_signalFence = m_juneAPI.InstanceCreateFence(m_juneInstance, &fenceDescriptor);
    }
}

void JuneGLESService1::work()
{
    spdlog::debug("gles service1 begin work");
    auto sharedMemories = getSharedMemories();
    if (sharedMemories.size() < 1)
        return;

    std::vector<EGLSyncKHR> waitEGLSyncs{};
    {
        std::vector<JuneFence> waitFences = getWaitFences();
        for (const auto& fence : waitFences)
        {
            JuneFenceEGLSyncExportDescriptor eglSyncExportDescriptor{};
            eglSyncExportDescriptor.chain.sType = JuneSType_FenceEGLSyncExportDescriptor;

            JuneFenceExportDescriptor descriptor{};
            descriptor.nextInChain = &eglSyncExportDescriptor.chain;
            descriptor.fence = fence;

            m_juneAPI.ApiContextExportFence(m_juneApiContext, &descriptor);

            if (!eglSyncExportDescriptor.eglSync)
            {
                spdlog::trace("EGLSync null in gles service 1: {:p}", eglSyncExportDescriptor.eglSync);
                continue;
            }
            waitEGLSyncs.push_back(eglSyncExportDescriptor.eglSync);
        }
    }

    for (auto count = 0; count < waitEGLSyncs.size(); ++count)
    {
        if (waitEGLSyncs[count] == nullptr)
        {
            spdlog::trace("EGLSync null in gles service 2: {:p}", waitEGLSyncs[count]);
            continue;
        }

        // EGLint eglResult = eglWaitSyncKHR(m_eglDisplay, waitEGLSyncs[count], EGL_SIGNALED_KHR);
        // CHECK_EGL_ERROR();
        // if (eglResult == EGL_FALSE)
        // {
        //     CHECK_GL_ERROR();
        //     spdlog::error("gles service 1 eglWaitSyncKHR failed");
        //     return;
        // }

        EGLint eglResult = eglClientWaitSyncKHR(m_eglDisplay, waitEGLSyncs[count], EGL_SYNC_FLUSH_COMMANDS_BIT, 0);
        CHECK_EGL_ERROR();
        if (eglResult == EGL_FALSE)
        {
            CHECK_GL_ERROR();
            spdlog::error("gles service 1 eglClientWaitSyncKHR failed");
            return;
        }

        spdlog::trace("EGLSync Destroyed in gles service 2: {:p}", waitEGLSyncs[count]);
        auto deleted = eglDestroySyncKHR(m_eglDisplay, waitEGLSyncs[count]);
        CHECK_EGL_ERROR();
        if (!deleted)
        {
            spdlog::error("Failed to destroy in gles service 2: {:p}", waitEGLSyncs[count]);
        }
    }

    // destroy
    if (m_eglSync != EGL_NO_SYNC_KHR)
    {
        EGLint value;
        eglGetSyncAttribKHR(m_eglDisplay, m_eglSync, EGL_SYNC_STATUS_KHR, &value);
        spdlog::trace("Current EGLSync status before waiting: {}", value);
        // EGL_SIGNALED_KHR       12530
        // EGL_UNSIGNALED_KHR     12531

        if (value == EGL_UNSIGNALED_KHR)
            eglClientWaitSyncKHR(m_eglDisplay, m_eglSync, 0, EGL_FOREVER_KHR);

        eglDestroySyncKHR(m_eglDisplay, m_eglSync);
        m_eglSync = EGL_NO_SYNC_KHR;
    }

    spdlog::debug("gles service1 begin access");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_eglImages[0]);

    GLuint fbo;
    {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures[0], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            spdlog::debug("Framebuffer is not complete");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &fbo);
            return;
        }
    }

    glUseProgram(m_programObject);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    // float r = dis(gen);
    float r = 1.0f;
    // float g = dis(gen);
    float g = 1.0f;
    float b = dis(gen);

    GLint colorLoc = glGetUniformLocation(m_programObject, "uColor");
    glUniform4f(colorLoc, r, g, b, 1.0f);

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

    GLint posLoc = glGetAttribLocation(m_programObject, "aPosition");
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, vertices);

    glViewport(0, 0, m_descriptor.width, m_descriptor.height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

    // glFlush();
    // glFinish();

    if (m_descriptor.windowHandle)
    {
        spdlog::debug("gles service1 is rendered in swapbuffer.");
        eglSwapBuffers(m_eglDisplay, m_eglSurface);
    }
    else
    {
        spdlog::debug("gles service1 is rendered in pbuffer.");
    }

    // create EGLSync
    {
        EGLint attribs[] = {
            EGL_SYNC_NATIVE_FENCE_FD_ANDROID, EGL_NO_NATIVE_FENCE_FD_ANDROID,
            EGL_NONE
        };
        m_eglSync = eglCreateSyncKHR(m_eglDisplay, EGL_SYNC_NATIVE_FENCE_ANDROID, attribs);
        if (m_eglSync == EGL_NO_SYNC_KHR)
        {
            spdlog::error("Failed to create a EGLSync");
            return;
        }

        spdlog::trace("Succeed create the EGLSync");

        // flush need to be called to make sure the sync object is created.
        glFlush();
    }

    spdlog::debug("gles service1 end access");

    // reset
    {
        int eglSyncFD = static_cast<int>(eglDupNativeFenceFDANDROID(m_eglDisplay, m_eglSync));
        if (eglSyncFD != EGL_NO_NATIVE_FENCE_FD_ANDROID)
        {
            JuneFenceSyncFDResetDescriptor syncFDResetDescriptor{};
            syncFDResetDescriptor.chain.sType = JuneSType_FenceSyncFDResetDescriptor;
            syncFDResetDescriptor.syncFD = eglSyncFD;

            JuneFenceResetDescriptor descriptor{};
            descriptor.nextInChain = &syncFDResetDescriptor.chain;

            m_juneAPI.FenceReset(m_signalFence, &descriptor);
        }
        else
        {
            spdlog::error("Failed to duplicate sync FD from EGLSync");
        }
    }

    glDisableVertexAttribArray(posLoc);
    glDeleteFramebuffers(1, &fbo);

    spdlog::debug("gles service1 end work");
}

void JuneGLESService1::end()
{
    for (auto& eglImage : m_eglImages)
    {
        if (eglImage)
        {
            eglDestroyImageKHR(m_eglDisplay, eglImage);
            CHECK_EGL_ERROR();
        }
    }
    m_eglImages.clear();

    for (auto& texture : m_textures)
    {
        if (texture)
        {
            glDeleteTextures(1, &texture);
            CHECK_GL_ERROR();
        }
    }

    JuneGLESService::end();
}

void JuneGLESService1::addSharedMemory(JuneSharedMemory sharedMemory)
{
    std::lock_guard<std::mutex> lock(m_sharedMemoryMutex);

    m_sharedMemories.push_back(sharedMemory);

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
        juneResourceDescriptor.sharedMemory = sharedMemory;

        m_juneAPI.ApiContextCreateResource(m_juneApiContext, &juneResourceDescriptor);

        m_eglImages.push_back(eglImageResultInfo.eglImage);
        m_eglClientBuffers.push_back(eglImageResultInfo.eglClientBuffer);
    }
}

} // namespace jipu
