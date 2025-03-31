#include "june_gles_service.h"

#include <stdexcept>

namespace jipu
{

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
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_NONE
        };

        EGLint numConfigs;
        if (!eglChooseConfig(m_eglDisplay, configAttribs, &m_eglConfig, 1, &numConfigs) || numConfigs < 1)
        {
            throw std::runtime_error("Failed to choose EGL config");
        }

        if (!m_descriptor.windowHandle)
        {
            EGLint pbufferAttribs[] = {
                EGL_WIDTH,
                1,
                EGL_HEIGHT,
                1,
                EGL_NONE,
            };
            m_eglSurface = eglCreatePbufferSurface(m_eglDisplay, m_eglConfig, pbufferAttribs);
            if (m_eglSurface == EGL_NO_SURFACE)
            {
                throw std::runtime_error("Failed to create EGL surface");
            }
        }
        else
        {
            // TODO: use ANativeWindow
            // ANativeWindow* window = static_cast<ANativeWindow*>(windowHandle);
        }

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
    }

    // initialize June
    {
        JuneInstanceDescriptor juneInstanceDescriptor{};
        m_juneInstance = m_juneAPI.CreateInstance(&juneInstanceDescriptor);

        JuneGLESApiContextDescriptor juenGLESApiContextDescriptor{};
        juenGLESApiContextDescriptor.chain.sType = JuneSType_GLESApiContext;
        juenGLESApiContextDescriptor.display = m_eglDisplay;
        juenGLESApiContextDescriptor.context = m_eglContext;

        JuneApiContextDescriptor juneApiContextDescriptor{
            .nextInChain = &juenGLESApiContextDescriptor.chain
        };
        m_juneApiContext = m_juneAPI.InstanceCreateApiContext(m_juneInstance, &juneApiContextDescriptor);
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

} // namespace jipu
