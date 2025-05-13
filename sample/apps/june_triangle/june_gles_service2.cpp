#include "june_gles_service2.h"

#include "file.h"
#include <spdlog/spdlog.h>

namespace jipu
{

namespace
{

const char* vertexShaderSource = R"(#version 300 es
in vec4 aPosition;
in vec2 aTexCoord;
out vec2 vTexCoord;

void main()
{
    gl_Position = aPosition;
    vTexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(#version 300 es
precision mediump float;
in vec2 vTexCoord;
uniform sampler2D uTexture;
out vec4 fragColor;

void main()
{
    vec4 color = texture(uTexture, vTexCoord);
    fragColor = vec4(color.rgb - 0.00, 1.0);
}
)";

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
        spdlog::error("Failed to compile shader. compiled: {}", compiled);
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = (char*)malloc(infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            spdlog::error("Failed to compile shader: {}", infoLog);
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
        m_signalFence = m_juneAPI.ApiContextCreateFence(m_juneApiContext, &fenceDescriptor);
    }
}

void JuneGLESService2::work()
{
    auto sharedMemories = getSharedMemories();
    if (sharedMemories.size() < 2)
        return;

    {
        std::vector<EGLSyncKHR> waitEGLSyncs{};
        std::vector<JuneFence> waitFences = getWaitFences();

        spdlog::trace("Charles Try to get sync object in gles service2.");
        for (const auto& fence : waitFences)
        {
            JuneFenceEGLSyncExportDescriptor eglSyncExportDescriptor{};
            eglSyncExportDescriptor.chain.sType = JuneSType_FenceEGLSyncExportDescriptor;

            JuneFenceExportDescriptor descriptor{};
            descriptor.nextInChain = &eglSyncExportDescriptor.chain;
            descriptor.fence = fence;

            m_juneAPI.ApiContextExportFence(m_juneApiContext, &descriptor);

            if (eglSyncExportDescriptor.eglSync)
            {
                waitEGLSyncs.push_back(eglSyncExportDescriptor.eglSync);
            }
        }

        for (auto count = 0; count < waitEGLSyncs.size(); ++count)
        {
            if (waitEGLSyncs[count] == nullptr)
            {
                spdlog::trace("Charles EGLSync null in gles service 2: {:p}", waitEGLSyncs[count]);
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

            spdlog::trace("Charles EGLSync Destroyed in gles service 2: {:p}", waitEGLSyncs[count]);
            auto deleted = eglDestroySyncKHR(m_eglDisplay, waitEGLSyncs[count]);
            CHECK_EGL_ERROR();
            if (!deleted)
            {
                spdlog::error("Charles Failed to destroy in gles service 2: {:p}", waitEGLSyncs[count]);
            }
        }
    }

    spdlog::debug("gles service2 begin access");

    // ──────────────────────── 1. 샘플링용 텍스처 설정 ────────────────────────
    glActiveTexture(GL_TEXTURE0); // ★ 샘플링 유닛 선택
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_eglImages[0]);

    // ──────────────────────── 2. 렌더 타깃 텍스처(FBO) 설정 ────────────────────────
    glActiveTexture(GL_TEXTURE1); // ★ 렌더 타깃 유닛 선택
    glBindTexture(GL_TEXTURE_2D, m_textures[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 권장: 파라미터 명시
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_eglImages[1]);

    // 렌더링을 위해 FBO 생성 및 텍스처 부착
    GLuint fbo;
    {
        glGenFramebuffers(1, &fbo);

        CHECK_GL_ERROR();
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        CHECK_GL_ERROR();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures[1], 0);

        CHECK_GL_ERROR();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            spdlog::debug("Framebuffer is not complete");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &fbo);
            return;
        }
    }

    glUseProgram(m_programObject);
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

    GLint posLoc = glGetAttribLocation(m_programObject, "aPosition");
    CHECK_GL_ERROR();
    GLint texLoc = glGetAttribLocation(m_programObject, "aTexCoord");
    CHECK_GL_ERROR();
    glEnableVertexAttribArray(posLoc);
    CHECK_GL_ERROR();
    glEnableVertexAttribArray(texLoc);
    CHECK_GL_ERROR();
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    CHECK_GL_ERROR();
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
    CHECK_GL_ERROR();

    GLint texUniform = glGetUniformLocation(m_programObject, "uTexture");
    CHECK_GL_ERROR();
    glUniform1i(texUniform, 0);
    CHECK_GL_ERROR();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);

    glViewport(0, 0, m_descriptor.width, m_descriptor.height);
    CHECK_GL_ERROR();
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // CHECK_GL_ERROR();
    // glClear(GL_COLOR_BUFFER_BIT);
    // CHECK_GL_ERROR();

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
        CHECK_EGL_ERROR();
    }
    else
    {
        spdlog::debug("gles service2 is rendered in pbuffer.");
    }

    spdlog::debug("gles service2 end access");

    {
        JuneFenceResetDescriptor descriptor{};
        m_juneAPI.FenceReset(m_signalFence, &descriptor);
    }

    glDisableVertexAttribArray(posLoc);
    CHECK_GL_ERROR();
    glDisableVertexAttribArray(texLoc);
    CHECK_GL_ERROR();
    glDeleteFramebuffers(1, &fbo);
    CHECK_GL_ERROR();
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
            CHECK_GL_ERROR();
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

} // namespace jipu
