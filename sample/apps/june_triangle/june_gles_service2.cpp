#include "june_gles_service2.h"

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

// Vertex Shader 소스 (화면 전체를 덮기 위한 정점 데이터 사용)
const char* vertexShaderSource =
    "attribute vec2 aPosition;\n"
    "attribute vec2 aTexCoord;\n"
    "varying vec2 vTexCoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPosition, 0.0, 1.0);\n"
    "    vTexCoord = aTexCoord;\n"
    "}\n";

// Fragment Shader 소스 (external texture 확장을 사용)
const char* fragmentShaderSource =
    "#extension GL_OES_EGL_image_external : require\n"
    "precision mediump float;\n"
    "varying vec2 vTexCoord;\n"
    "uniform samplerExternalOES sTexture;\n"
    "void main() {\n"
    "    gl_FragColor = texture2D(sTexture, vTexCoord);\n"
    "}\n";

// 쉐이더 컴파일 함수
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

// 쉐이더 프로그램 생성 함수
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

JuneGLESService2::JuneGLESService2(const JuneServiceDescriptor& descriptor)
    : JuneGLESService(descriptor)
{
}

JuneGLESService2::~JuneGLESService2()
{
}

void JuneGLESService2::work()
{
    if (!m_juneApiMemory)
        return;

    // 1. 쉐이더 프로그램 생성 및 사용
    GLuint program = createProgram(vertexShaderSource, fragmentShaderSource);
    if (program == 0)
    {
        spdlog::error("프로그램 생성 실패");
        return;
    }
    glUseProgram(program);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_eglImage);

    // 텍스처 파라미터 설정
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 3. 정점 데이터 설정: 화면 전체를 덮는 quad (좌표: -1~1, 텍스처 좌표: 0~1)
    // 데이터 구성: {x, y, u, v}
    GLfloat vertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f,  // 좌상단
        -1.0f, -1.0f, 0.0f, 1.0f, // 좌하단
        1.0f, 1.0f, 1.0f, 0.0f,   // 우상단
        1.0f, -1.0f, 1.0f, 1.0f   // 우하단
    };

    // 4. 정점 속성 위치 가져오기
    GLint posAttrib = glGetAttribLocation(program, "aPosition");
    CHECK_GL_ERROR();
    GLint texAttrib = glGetAttribLocation(program, "aTexCoord");
    CHECK_GL_ERROR();

    // 5. 정점 속성 활성화 및 포인터 설정
    glEnableVertexAttribArray(posAttrib);
    CHECK_GL_ERROR();
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices);
    CHECK_GL_ERROR();

    glEnableVertexAttribArray(texAttrib);
    CHECK_GL_ERROR();
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices + 2);
    CHECK_GL_ERROR();

    // 6. 텍스처 유닛 설정: samplerExternalOES에 0번 텍스처 유닛 사용
    GLint samplerLoc = glGetUniformLocation(program, "sTexture");
    CHECK_GL_ERROR();
    glUniform1i(samplerLoc, 0);

    glViewport(0, 0, m_descriptor.width / 2, m_descriptor.height);
    CHECK_GL_ERROR();

    // 8. 화면 클리어
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    CHECK_GL_ERROR();
    glClear(GL_COLOR_BUFFER_BIT);
    CHECK_GL_ERROR();

    // 9. full-screen quad 렌더링 (Triangle Strip 사용)
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    CHECK_GL_ERROR();

    if (m_descriptor.windowHandle)
    {
        spdlog::debug("service2 is rendered in swapbuffer.");
        eglSwapBuffers(m_eglDisplay, m_eglSurface);
        CHECK_GL_ERROR();
    }
    else
    {
        spdlog::debug("service2 is rendered in pbuffer.");
    }
}

void JuneGLESService2::shareMemory(JuneSharedMemory sharedMemory)
{
    setJuneSharedMemory("memory1", sharedMemory);

    // Create Api Memory
    {
        JuneApiMemoryDescriptor juneApiMemoryDescriptor{};
        juneApiMemoryDescriptor.nextInChain = nullptr;
        juneApiMemoryDescriptor.sharedMemory = sharedMemory;

        m_juneApiMemory = m_juneAPI.ApiContextCreateApiMemory(m_juneApiContext, &juneApiMemoryDescriptor);
    }

    // Create Resource
    {
        JuneResourceEGLImageDescriptor juneResourceEGLImageDescriptor{};
        juneResourceEGLImageDescriptor.chain.sType = JuneStype_EGLImageResourceDescriptor;
        JuneResourceDescriptor juneResourceDescriptor{};
        juneResourceDescriptor.nextInChain = &juneResourceEGLImageDescriptor.chain;

        m_eglImage = static_cast<EGLImageKHR>(m_juneAPI.ApiMemoryCreateResource(m_juneApiMemory, &juneResourceDescriptor));
    }
}

} // namespace jipu
