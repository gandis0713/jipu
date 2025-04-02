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

const char* vertexShaderSource2 =
    "attribute vec4 aPosition;            \n"
    "attribute vec2 aTexCoord;            \n"
    "varying vec2 vTexCoord;              \n"
    "void main() {                      \n"
    "    gl_Position = aPosition;         \n"
    "    vTexCoord = aTexCoord;           \n"
    "}                                    \n";
const char* fragmentShaderSource2 =
    //     "precision mediump float;             \n"
    //     "varying vec2 vTexCoord;              \n"
    //     "uniform sampler2D uTexture;          \n"
    //     "void main() {                        \n"
    //     "    gl_FragColor = texture2D(uTexture, vTexCoord); \n"
    //     "}                                    \n";
    "precision mediump float;                                        \n"
    "varying vec2 vTexCoord;                                             \n"
    "uniform sampler2D uTexture;                                         \n"
    "const int texWidth = 160;                                             \n"
    "const int texHeight = 160;                                            \n"
    "void main() {                                                     \n"
    "    // 기준 색상을 texture의 첫번째 texel에서 샘플링                         \n"
    "    vec4 refColor = texture2D(uTexture, vec2(0.5/float(texWidth),      \n"
    "                                           0.5/float(texHeight)));    \n"
    "    bool isUniform = true;                                          \n"
    "    for (int y = 0; y < texHeight; y++) {                           \n"
    "        for (int x = 0; x < texWidth; x++) {                        \n"
    "            vec2 coord = vec2((float(x) + 0.5) / float(texWidth),    \n"
    "                                (float(y) + 0.5) / float(texHeight));   \n"
    "            vec4 currentColor = texture2D(uTexture, coord);         \n"
    "            // 기준 색상과의 차이가 아주 작으면 동일하다고 판단                     \n"
    "            if (distance(currentColor, refColor) > 0.001) {         \n"
    "                isUniform = false;                                \n"
    "            }                                                     \n"
    "        }                                                         \n"
    "    }                                                             \n"
    "    // texture의 모든 색상이 동일하면 기준 색상을, 그렇지 않으면 원래 texture 색상을 출력       \n"
    "    if (isUniform) {                                              \n"
    "         gl_FragColor = texture2D(uTexture, vTexCoord);                                 \n"
    "    } else {                                                      \n"
    "         gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);           \n"
    "    }                                                             \n"
    "}                                                                 \n";

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

    m_programObject2 = createProgram(vertexShaderSource2, fragmentShaderSource2);
    if (m_programObject2 == 0)
    {
        throw std::runtime_error("Failed to create program");
    }

    glGenTextures(1, &m_texture);
    CHECK_GL_ERROR();
}

void JuneGLESService2::work()
{
    if (!m_juneApiMemory)
        return;

    JuneApiMemoryBeginAccessDescriptor descriptor{};
    m_juneAPI.ApiMemoryBeginAccess(m_juneApiMemory, &descriptor);
    spdlog::debug("service2 begin access");

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
        spdlog::debug("service2 is rendered in swapbuffer.");
        eglSwapBuffers(m_eglDisplay, m_eglSurface);
        CHECK_GL_ERROR();
    }
    else
    {
        spdlog::debug("service2 is rendered in pbuffer.");
    }

    spdlog::debug("service2 end access");
    JuneApiMemoryEndAccessDescriptor endDescriptor{};
    m_juneAPI.ApiMemoryEndAccess(m_juneApiMemory, &endDescriptor);

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
    return {};
}

void JuneGLESService2::setSharedObjects(const JuneServiceShareObjects& sharedObjects)
{
    m_sharedObjects = sharedObjects;

    // Create ApiMemory and connect
    {
        JuneApiMemoryDescriptor juneApiMemoryDescriptor{};
        juneApiMemoryDescriptor.nextInChain = nullptr;
        juneApiMemoryDescriptor.sharedMemory = m_sharedObjects.sharedMemory;

        m_juneApiMemory = m_juneAPI.ApiContextCreateApiMemory(m_juneApiContext, &juneApiMemoryDescriptor);

        for (const auto& sharedApiMemory : m_sharedObjects.apiMemories)
        {
            m_juneAPI.ApiMemoryConnect(sharedApiMemory, m_juneApiMemory);
            m_juneAPI.ApiMemoryConnect(m_juneApiMemory, sharedApiMemory);
        }
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
