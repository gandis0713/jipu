#include "june_gles_service.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
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
    {
        spdlog::error("Failed to compile vertex shader");
        return 0;
    }

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader)
    {
        spdlog::error("Failed to compile fragment shader");
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program == 0)
    {
        spdlog::error("Failed to create program");
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        spdlog::error("Failed to link program. linked: {}", linked);
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

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

JuneGLESService::JuneGLESService(const JuneServiceDescriptor& descriptor)
    : JuneService(descriptor)
{
}

JuneGLESService::~JuneGLESService()
{
}

void JuneGLESService::begin()
{
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_eglDisplay != EGL_NO_DISPLAY)
    {
        if (!eglInitialize(m_eglDisplay, nullptr, nullptr))
        {
            throw std::runtime_error("Failed to initialize EGL");
        }
    }
    else
    {
        throw std::runtime_error("Failed to get EGL display");
    }

    // initialize EGL
    {
        EGLint configAttribs[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, m_descriptor.sharingData->windowHandle ? EGL_WINDOW_BIT : EGL_PBUFFER_BIT,
            EGL_NONE
        };

        EGLint numConfigs;
        if (!eglChooseConfig(m_eglDisplay, configAttribs, &m_eglConfig, 1, &numConfigs) || numConfigs < 1)
        {
            throw std::runtime_error("Failed to choose EGL config");
        }

        createEGLSurface();

        EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };
        m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, contextAttribs);
        if (m_eglContext == EGL_NO_CONTEXT)
        {
            throw std::runtime_error("Failed to create EGL context");
        }

        if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))
        {
            throw std::runtime_error("Failed to make EGL context current");
        }

        eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
        if (!eglDestroyImageKHR)
        {
            throw std::runtime_error("eglDestroyImageKHR function pointer acquisition failed");
        }

        glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");
        if (!glEGLImageTargetTexture2DOES)
        {
            throw std::runtime_error("glEGLImageTargetTexture2DOES function pointer acquisition failed");
        }

        eglDupNativeFenceFDANDROID = (PFNEGLDUPNATIVEFENCEFDANDROIDPROC)eglGetProcAddress("eglDupNativeFenceFDANDROID");
        if (!eglDupNativeFenceFDANDROID)
        {
            throw std::runtime_error("eglDupNativeFenceFDANDROID function pointer acquisition failed");
        }

        eglCreateSyncKHR = (PFNEGLCREATESYNCKHRPROC)eglGetProcAddress("eglCreateSyncKHR");
        if (!eglCreateSyncKHR)
        {
            throw std::runtime_error("eglCreateSyncKHR function pointer acquisition failed");
        }

        eglDestroySyncKHR = (PFNEGLDESTROYSYNCKHRPROC)eglGetProcAddress("eglDestroySyncKHR");
        if (!eglDestroySyncKHR)
        {
            throw std::runtime_error("eglDestroySyncKHR function pointer acquisition failed");
        }

        eglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)eglGetProcAddress("eglClientWaitSyncKHR");
        if (!eglClientWaitSyncKHR)
        {
            throw std::runtime_error("eglClientWaitSyncKHR function pointer acquisition failed");
        }

        eglGetSyncAttribKHR = (PFNEGLGETSYNCATTRIBKHRPROC)eglGetProcAddress("eglGetSyncAttribKHR");
        if (!eglGetSyncAttribKHR)
        {
            throw std::runtime_error("eglGetSyncAttribKHR function pointer acquisition failed");
        }

        eglWaitSyncKHR = (PFNEGLWAITSYNCKHRPROC)eglGetProcAddress("eglWaitSyncKHR");
        if (!eglWaitSyncKHR)
        {
            throw std::runtime_error("eglWaitSyncKHR function pointer acquisition failed");
        }
    }
}

void JuneGLESService::end()
{
    eglDestroyContext(m_eglDisplay, m_eglContext);
    eglDestroySurface(m_eglDisplay, m_eglSurface);
    eglTerminate(m_eglDisplay);

    m_eglDisplay = EGL_NO_DISPLAY;
    m_eglSurface = EGL_NO_SURFACE;
    m_eglContext = EGL_NO_CONTEXT;
    m_eglConfig = EGL_NO_CONFIG_KHR;
}

void JuneGLESService::createApiContext(const std::string& label)
{
    JuneGLESContextDescriptor juneGLESContextDescriptor{};
    juneGLESContextDescriptor.chain.sType = JuneSType_GLESContext;
    juneGLESContextDescriptor.display = m_eglDisplay;
    juneGLESContextDescriptor.context = m_eglContext;

    JuneApiContextDescriptor juneApiContextDescriptor;
    juneApiContextDescriptor.nextInChain = &juneGLESContextDescriptor.chain;
    juneApiContextDescriptor.label.data = label.data();
    juneApiContextDescriptor.label.length = label.length();
    m_juneApiContext = m_juneAPI.InstanceCreateApiContext(m_juneInstance, &juneApiContextDescriptor);
}

} // namespace jipu
