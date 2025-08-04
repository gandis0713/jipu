#include "june_gles_service2.h"

#include "file.h"
#include <spdlog/spdlog.h>

namespace jipu
{

namespace
{

const char* vertexShaderSource = R"(#version 320 es
in vec4 aPosition;
// flat out int vDiscard;
in vec2 aTexCoord;
out vec2 vTexCoord;

// uniform sampler2D uTexture;

void main()
{
    gl_Position = aPosition;
    vTexCoord = aTexCoord;
    
    // int width = 1080;
    // int height = 2340;

    // float uInvLargestDim = 1.0f / float(max(width, height));
    // float lod = 1.0 / uInvLargestDim;   // log2(maxDim) 를 컴파일 타임 fold
    // vec4   sample1x1 = textureLod(uTexture, vec2(0.5), lod);
    // vec4   refTexel  = texelFetch(uTexture, ivec2(0, 0), 0);
    // bool   isMono    = all(equal(sample1x1, refTexel));

    // vDiscard = isMono ? 0 : 1;
}
)";

const char* fragmentShaderSource = R"(#version 320 es
precision mediump float;
// flat in int vDiscard;
in vec2 vTexCoord;
out vec4 fragColor;

uniform sampler2D uTexture;

void main()
{
    // if(vDiscard == 1)
    // {
    //     discard;
    // }

    fragColor = texture(uTexture, vTexCoord);
}
)";

} // namespace

JuneGLESService2::JuneGLESService2(const JuneServiceDescriptor& descriptor)
    : JuneGLESService(descriptor)
{
    m_memoryNode = std::make_unique<JuneMemoryNode>(JuneMemoryNodeDescriptor{
        .label = "gles service2 memory node",
        .sharedMemory = nullptr,
        .waitAccessCount = 1 });
}

JuneGLESService2::~JuneGLESService2()
{
}

void JuneGLESService2::begin()
{
    JuneGLESService::begin();

    {
        // std::vector<char> vertex = utils::readFile(m_descriptor.appDir / "gles_service2_vert.glsl", m_descriptor.appHandle);
        // std::vector<char> fragment = utils::readFile(m_descriptor.appDir / "gles_service2_frag.glsl", m_descriptor.appHandle);
        // m_programObject = createProgram(vertex.data(), fragment.data());
        m_programObject = createProgram(vertexShaderSource, fragmentShaderSource);
        if (m_programObject == 0)
        {
            throw std::runtime_error("Failed to create program 2");
        }

        glBindAttribLocation(m_programObject, 0, "aPosition");
        glBindAttribLocation(m_programObject, 1, "aTexCoord");

        GLuint texture1;
        glGenTextures(1, &texture1);
        m_textures.push_back(texture1);

        GLuint texture2;
        glGenTextures(1, &texture2);
        m_textures.push_back(texture2);
    }
    std::string label = "gles service2";

    createInstance(label);
    createApiContext(label);

    // Create Fence
    {
        std::string label = "gles service2 fence";
        JuneFenceCreateDescriptor fenceDescriptor;
        fenceDescriptor.label.data = label.c_str();
        fenceDescriptor.label.length = static_cast<uint32_t>(label.length());
        fenceDescriptor.type = JuneFenceType_SyncFD;
        m_signalFence = m_juneAPI.InstanceCreateFence(m_juneInstance, &fenceDescriptor);
    }
}

void JuneGLESService2::work()
{
    auto sharedMemories = getSharedMemories();
    if (sharedMemories.size() < 1)
        return;

    m_memoryNode->beginAccess();
    spdlog::debug("gles service2 begin access");

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
                continue;
            }
            spdlog::trace("EGLSync in gles service 2: {:p}", eglSyncExportDescriptor.eglSync);
            waitEGLSyncs.push_back(eglSyncExportDescriptor.eglSync);
        }
    }

    for (auto count = 0; count < waitEGLSyncs.size(); ++count)
    {
        // EGLint eglResult = eglWaitSyncKHR(m_eglDisplay, waitEGLSyncs[count], EGL_SIGNALED_KHR);
        // CHECK_EGL_ERROR();
        // if (eglResult == EGL_FALSE)
        // {
        //     CHECK_GL_ERROR();
        //     // spdlog::error("gles service 1 eglWaitSyncKHR failed");
        //     return;
        // }

        EGLint eglResult = eglClientWaitSyncKHR(m_eglDisplay, waitEGLSyncs[count], EGL_SYNC_FLUSH_COMMANDS_BIT, 0);
        CHECK_EGL_ERROR(eglClientWaitSyncKHR);
        if (eglResult == EGL_FALSE)
        {
            spdlog::error("gles service 1 eglClientWaitSyncKHR failed");
            return;
        }

        auto deleted = eglDestroySyncKHR(m_eglDisplay, waitEGLSyncs[count]);
        CHECK_EGL_ERROR(eglDestroySyncKHR);
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
        // EGL_SIGNALED_KHR       12530
        // EGL_UNSIGNALED_KHR     12531

        if (value == EGL_UNSIGNALED_KHR)
            eglClientWaitSyncKHR(m_eglDisplay, m_eglSync, 0, EGL_FOREVER_KHR);

        eglDestroySyncKHR(m_eglDisplay, m_eglSync);
        m_eglSync = EGL_NO_SYNC_KHR;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_eglImages[0]);

    glUseProgram(m_programObject);

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
    GLint texLoc = glGetAttribLocation(m_programObject, "aTexCoord");
    glEnableVertexAttribArray(posLoc);
    glEnableVertexAttribArray(texLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, texCoords);

    GLint texUniform = glGetUniformLocation(m_programObject, "uTexture");
    glUniform1i(texUniform, 0);

    glViewport(0, 0, m_descriptor.sharingData->width, m_descriptor.sharingData->height);
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

    // glFlush();
    // glFinish();

    eglSwapBuffers(m_eglDisplay, m_eglSurface);

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

        // flush need to be called to make sure the sync object is created.
        glFlush();
    }

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

    spdlog::debug("gles service2 end access");
    m_memoryNode->endAccess();

    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(texLoc);

    // spdlog::debug("gles service2 end work");
}

void JuneGLESService2::end()
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
            // CHECK_GL_ERROR();
        }
    }

    JuneGLESService::end();
}

void JuneGLESService2::addSharedMemory(JuneSharedMemory sharedMemory)
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

void JuneGLESService2::createEGLSurface()
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
