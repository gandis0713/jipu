#include "june_gles_litert_service.h"

#include "file.h"
#include "image.h"
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <random>
#include <spdlog/spdlog.h>
#include <thread>

#include "pixel_converter.h"

namespace jipu
{

namespace
{

// 정점 셰이더 소스
const char* vertexShaderSource =
    "#version 100\n"
    "attribute vec4 a_position;\n"
    "attribute vec2 a_texCoord;\n"
    "varying vec2 v_texCoord;\n"
    "void main() {\n"
    "    gl_Position = a_position;\n"
    "    v_texCoord = a_texCoord;\n"
    "}\n";

// 프래그먼트 셰이더 소스
const char* fragmentShaderSource =
    // "#version 100\n"
    // "precision mediump float;\n"
    // "varying vec2 v_texCoord;\n"
    // "uniform sampler2D u_texture;\n"
    // "uniform sampler2D u_texture_mask;\n"
    // "void main() {\n"
    // "    vec3 color = texture2D(u_texture, v_texCoord).rgb;\n"
    // "    float mask = texture2D(u_texture_mask, v_texCoord).r;\n"
    // "    \n"
    // "    if (mask > 0.5) {\n"
    // "        gl_FragColor = vec4(color, 1.0);\n" // 완전 불투명
    // "    } else {\n"
    // "        discard;\n" // 픽셀 버리기 (완전히 제거)
    // "    }\n"
    // "}\n";
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "uniform sampler2D u_texture_mask;\n" // 마스크 텍스처 추가
    "void main() {\n"
    "    vec3 color = texture2D(u_texture, v_texCoord).rgb;\n"     // RGB 값만 추출
    "    float alpha = texture2D(u_texture_mask, v_texCoord).r;\n" // 마스크의 R 채널을 알파값으로 사용
    "    gl_FragColor = vec4(color, alpha);\n"                     // 최종 색상에 마스크를 알파값으로 적용
    "}\n";

void updateTexture(GLuint textureId, unsigned char* imageData, int width, int height, int channels)
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    CHECK_GL_ERROR(glBindTexture);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, imageData);
    CHECK_GL_ERROR(glTexSubImage2D);
    // glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR(glBindTexture);
}

} // namespace

JuneGLESLiteRtService::JuneGLESLiteRtService(const JuneServiceDescriptor& descriptor)
    : JuneGLESService(descriptor)
{
    //    m_androidCamera.setResolution(480, 640);
    //    m_androidCamera.setCallback([this](AImage* image) {
    //        AHardwareBuffer* hardwareBuffer = nullptr;
    //        auto status = AImage_getHardwareBuffer(image, &hardwareBuffer);
    //        if (status != AMEDIA_OK || !hardwareBuffer)
    //        {
    //            spdlog::error("Failed to get hardware buffer from image, status: {}", static_cast<uint32_t>(status));
    //            return;
    //        }
    //
    //        {
    //            // 이미지 크기 얻기
    //            int32_t width, height;
    //            if (AImage_getWidth(image, &width) != AMEDIA_OK ||
    //                AImage_getHeight(image, &height) != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get image dimensions");
    //                return;
    //            }
    //
    //            // 플레인 수 확인
    //            int32_t numPlanes;
    //            if (AImage_getNumberOfPlanes(image, &numPlanes) != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get number of planes");
    //                return;
    //            }
    //
    //            if (numPlanes != 3)
    //            {
    //                spdlog::error("Expected 3 planes for YUV_420_888, got %d", numPlanes);
    //                return;
    //            }
    //
    //            // 각 플레인의 데이터 포인터와 스트라이드 정보 얻기
    //            uint8_t* yData = nullptr;
    //            uint8_t* uData = nullptr;
    //            uint8_t* vData = nullptr;
    //            int32_t yPixelStride, yRowStride;
    //            int32_t uPixelStride, uRowStride;
    //            int32_t vPixelStride, vRowStride;
    //            int32_t yDataLen, uDataLen, vDataLen;
    //
    //            // Y 플레인 (인덱스 0)
    //            auto result = AImage_getPlaneData(image, 0, &yData, &yDataLen);
    //            if (result != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get Y plane data, status: {}", static_cast<uint32_t>(result));
    //                return;
    //            }
    //
    //            result = AImage_getPlanePixelStride(image, 0, &yPixelStride);
    //            if (result != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get Y plane pixel stride, status: {}", static_cast<uint32_t>(result));
    //                return;
    //            }
    //
    //            result = AImage_getPlaneRowStride(image, 0, &yRowStride);
    //            if (result != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get Y plane row stride, status: {}", static_cast<uint32_t>(result));
    //                return;
    //            }
    //
    //            // U 플레인 (인덱스 1)
    //            result = AImage_getPlaneData(image, 1, &uData, &uDataLen);
    //            if (result != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get U plane data, status: {}", static_cast<uint32_t>(result));
    //                return;
    //            }
    //
    //            result = AImage_getPlanePixelStride(image, 1, &uPixelStride);
    //            if (result != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get U plane pixel stride, status: {}", static_cast<uint32_t>(result));
    //                return;
    //            }
    //
    //            result = AImage_getPlaneRowStride(image, 1, &uRowStride);
    //            if (result != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get U plane row stride, status: {}", static_cast<uint32_t>(result));
    //                return;
    //            }
    //
    //            // V 플레인 (인덱스 2)
    //            result = AImage_getPlaneData(image, 2, &vData, &vDataLen);
    //            if (result != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get V plane data, status: {}", static_cast<uint32_t>(result));
    //                return;
    //            }
    //
    //            result = AImage_getPlanePixelStride(image, 2, &vPixelStride);
    //            if (result != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get V plane pixel stride, status: {}", static_cast<uint32_t>(result));
    //                return;
    //            }
    //
    //            result = AImage_getPlaneRowStride(image, 2, &vRowStride);
    //            if (result != AMEDIA_OK)
    //            {
    //                spdlog::error("Failed to get V plane row stride, status: {}", static_cast<uint32_t>(result));
    //                return;
    //            }
    //
    //            {
    //                std::lock_guard<std::mutex> lock(m_frameMutex);
    //                m_currentImage = image;
    //                m_currentHardwareBuffer = hardwareBuffer;
    //
    //                m_frameData.clear();
    //                m_frameData.resize(width * height * 3);
    //
    //                yuv420toRgb(yData, uData, vData, width, height,
    //                            yRowStride, uRowStride, uPixelStride, m_frameData.data());
    //
    //                // spdlog::info(" yRowStride: {}, uRowStride: {}, vRowStride: {}, width: {}, height: {}",
    //                //              yRowStride, uRowStride, vRowStride, width, height);
    //            }
    //        }
    //    });
    //    m_androidCamera.startCamera();
}

JuneGLESLiteRtService::~JuneGLESLiteRtService()
{
}

void JuneGLESLiteRtService::begin()
{
    JuneGLESService::begin();

    // Create and compile shaders, create program
    {
        m_programObject = createProgram(vertexShaderSource, fragmentShaderSource);

        if (m_programObject == 0)
        {
            throw std::runtime_error("Failed to create program 1");
        }
    }

    // Create vertex buffer object (VBO) and set up vertex attributes
    {
        // 정점 데이터 (위치 + 텍스처 좌표)
        float vertices[] = {
            // 위치 (x, y)    텍스처 좌표 (u, v)
            -1.0f, -1.0f, 0.0f, 1.0f, // 왼쪽 아래
            1.0f, -1.0f, 1.0f, 1.0f,  // 오른쪽 아래
            -1.0f, 1.0f, 0.0f, 0.0f,  // 왼쪽 위
            1.0f, 1.0f, 1.0f, 0.0f    // 오른쪽 위
        };

        // VBO 생성
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    }

    {
        m_positionLoc = glGetAttribLocation(m_programObject, "a_position");
        m_texCoordLoc = glGetAttribLocation(m_programObject, "a_texCoord");
        m_textureLoc = glGetUniformLocation(m_programObject, "u_texture");
        m_textureMaskLoc = glGetUniformLocation(m_programObject, "u_texture_mask");
        CHECK_GL_ERROR(glGetUniformLocation)
    }

    m_liteRtInference = std::make_unique<LiteRtImageInference>(m_eglContext, m_eglDisplay);

    // Load model
    {
        // std::string modelPath = m_descriptor.sharingData->appDir / "deeplabv3.tflite";
        std::string modelPath = m_descriptor.sharingData->appDir / "mediapipe.tflite";

        std::vector<char> modelBuffer = utils::readFile(modelPath, m_descriptor.sharingData->appHandle);
        if (!m_liteRtInference->loadModel(modelBuffer))
        {
            spdlog::error("Failed to load model");
            return;
        }
    }

    int inputWidth = m_liteRtInference->getInputWidth();
    int inputHeight = m_liteRtInference->getInputHeight();
    int inputChannels = m_liteRtInference->getInputChannel();
    int outputWidth = m_liteRtInference->getOutputWidth();
    int outputHeight = m_liteRtInference->getOutputHeight();
    int outputChannels = m_liteRtInference->getOutputChannel();

    spdlog::info("Model loaded with input size: {}x{}, channels: {}, output size: {}x{}, channels: {}", inputWidth, inputHeight, inputChannels, outputWidth, outputHeight, outputChannels);

    // Create input image
    {
        std::vector<char> imageBuffer = utils::readFile(m_descriptor.sharingData->appDir / "man.png", m_descriptor.sharingData->appHandle);
        if (imageBuffer.empty())
        {
            spdlog::error("Failed to read image file");
            return;
        }

        m_image = std::make_unique<Image>(imageBuffer.data(), imageBuffer.size(), inputWidth, inputHeight, inputChannels);
        m_texture = createTexture(m_image->getPixels(), m_image->getWidth(), m_image->getHeight(), m_image->getChannel());

        m_liteRtInference->setInputImage(m_image.get());

        if (m_liteRtInference->getAcceleratorType() == LiteRtImageInference::AcceleratorType::kGPU)
        {
            auto preprocessed = m_liteRtInference->getPreprocessedData();
            if (preprocessed.empty())
            {
                spdlog::error("Preprocessed data is empty");
                return;
            }

            auto liteRtInputGlBuffer = m_liteRtInference->getInputGlBuffer();
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, liteRtInputGlBuffer.id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, preprocessed.size() * sizeof(float), preprocessed.data(), GL_DYNAMIC_COPY);
        }
    }

    // Create output image
    {
        m_mask = std::make_unique<Image>();
        std::vector<unsigned char> maskPixels(outputWidth * outputHeight * outputChannels, 255); // Initialize with zeros
        m_mask->setPixels(maskPixels.data(), outputWidth, outputHeight, outputChannels);
        // m_textureMask = createTexture(m_mask->getPixels(), m_mask->getWidth(), m_mask->getHeight(), m_mask->getChannel());
    }

    // inference
    std::vector<uint8_t> result{};
    {
        result = m_liteRtInference->runInference();
        if (result.empty())
        {
            spdlog::error("Inference result is empty");
            return;
        }

        spdlog::info("Inference result size: {}", result.size());
        if (result.size() != outputWidth * outputHeight * outputChannels)
        {
            spdlog::error("Inference result size does not match mask image size: expected {}, got {}",
                          outputWidth * outputHeight * outputChannels, result.size());
            return;
        }

        m_mask->setPixels(result.data(), outputWidth, outputHeight, outputChannels);
        m_mask->convert(outputWidth, outputHeight, 3); // Convert to 3 channels
        // updateTexture(m_textureMask, m_mask->getPixels(), m_mask->getWidth(), m_mask->getHeight(), m_mask->getChannel());
        m_textureMask = createTexture(m_mask->getPixels(), m_mask->getWidth(), m_mask->getHeight(), m_mask->getChannel());
    }

    // std::string label = "gles service1";

    // createInstance(label);
    // createApiContext(label);
}

void JuneGLESLiteRtService::work()
{
    // AHardwareBuffer* currentHardwareBuffer = nullptr;
    // {
    //     std::lock_guard<std::mutex> lock(m_frameMutex);
    //     if (m_currentHardwareBuffer)
    //     {
    //         currentHardwareBuffer = m_currentHardwareBuffer;
    //         m_currentHardwareBuffer = nullptr;
    //     }
    // }

    // auto getEGLImageKHRFromAHardwareBuffer = [this](AHardwareBuffer* hardwareBuffer) -> EGLImageKHR {
    //     EGLImageKHR currentEGLImage = EGL_NO_IMAGE_KHR;
    //     if (hardwareBuffer)
    //     {
    //         // AHardwareBuffer를 EGLClientBuffer로 변환
    //         auto it = m_frames.find(hardwareBuffer);
    //         if (it == m_frames.end())
    //         {
    //             EGLClientBuffer eglClientBuffer = eglGetNativeClientBufferANDROID(hardwareBuffer);
    //             if (!eglClientBuffer)
    //             {
    //                 spdlog::error("Failed to get EGLClientBuffer from AHardwareBuffer");
    //                 return currentEGLImage;
    //             }

    //             EGLint imageAttribs[] = {
    //                 EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
    //                 EGL_NONE
    //             };

    //             EGLImageKHR eglImage = eglCreateImageKHR(
    //                 m_eglDisplay, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID,
    //                 eglClientBuffer, imageAttribs);

    //             if (eglImage == EGL_NO_IMAGE_KHR)
    //             {
    //                 spdlog::error("Failed to create EGLImage from AHardwareBuffer");
    //                 return currentEGLImage;
    //             }

    //             m_frames[hardwareBuffer] = eglImage;
    //             currentEGLImage = eglImage;
    //         }
    //         else
    //         {
    //             currentEGLImage = it->second;
    //         }
    //     }

    //     return currentEGLImage;
    // };

    // EGLImageKHR currentEGLImage = EGL_NO_IMAGE_KHR;
    // if (currentHardwareBuffer)
    // {
    //     currentEGLImage = getEGLImageKHRFromAHardwareBuffer(currentHardwareBuffer);
    //     if (currentEGLImage != EGL_NO_IMAGE_KHR)
    //     {
    //         if (m_liteRtInference->nextFrame(currentEGLImage))
    //         {
    //             spdlog::info("Frame processed successfully");
    //         }
    //         else
    //         {
    //             spdlog::error("Failed to process frame");
    //         }
    //         // 현재 EGLImage를 사용하여 후처리 작업 수행
    //         // 예: 텍스처 업데이트, 렌더링 등
    //         // updateTexture(m_texture, m_image->getPixels(), m_image->getWidth(), m_image->getHeight(), m_image->getChannel());
    //     }
    //     else
    //     {
    //         spdlog::error("No valid EGLImage available for processing");
    //     }

    //     {
    //         m_image->setPixels(m_frameData.data(), 640, 480, 3);
    //         m_image->convert(256, 256, 3);
    //         updateTexture(m_texture, m_image->getPixels(), m_image->getWidth(), m_image->getHeight(), m_image->getChannel());
    //     }

    //     std::vector<uint8_t> result{};
    //     // set input image and inference
    //     {
    //         m_liteRtInference->setInputImage(m_image.get());
    //         result = m_liteRtInference->runInference();
    //     }

    //     // Update mask texture with inference result
    //     {
    //         if (result.empty())
    //         {
    //             spdlog::error("Inference result is empty");
    //             return;
    //         }

    //         if (result.size() != m_liteRtInference->getOutputWidth() * m_liteRtInference->getOutputHeight())
    //         {
    //             spdlog::error("Inference result size does not match mask image size: expected {}, got {}",
    //                           m_liteRtInference->getOutputWidth() * m_liteRtInference->getOutputHeight(), result.size());
    //             return;
    //         }

    //         m_mask->setPixels(result.data(), m_liteRtInference->getOutputWidth(), m_liteRtInference->getOutputHeight(), m_liteRtInference->getOutputChannel());
    //         updateTexture(m_textureMask, m_mask->getPixels(), m_mask->getWidth(), m_mask->getHeight(), m_mask->getChannel());
    //     }
    // }

    // 화면 클리어
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    CHECK_GL_ERROR(glClear);

    // 알파 블렌딩 활성화
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_GL_ERROR(glEnable);

    // 프로그램 사용
    glUseProgram(m_programObject);
    CHECK_GL_ERROR(glUseProgram);

    // 버퍼 바인딩
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    CHECK_GL_ERROR(glBindBuffer);

    // 정점 어트리뷰트 설정
    glEnableVertexAttribArray(m_positionLoc);
    glVertexAttribPointer(m_positionLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    CHECK_GL_ERROR(glVertexAttribPointer);

    glEnableVertexAttribArray(m_texCoordLoc);
    glVertexAttribPointer(m_texCoordLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    CHECK_GL_ERROR(glVertexAttribPointer);

    // 메인 텍스처 바인딩 (텍스처 유닛 0)
    glActiveTexture(GL_TEXTURE0);
    CHECK_GL_ERROR(glActiveTexture)
    glBindTexture(GL_TEXTURE_2D, m_texture);
    CHECK_GL_ERROR(glBindTexture)
    glUniform1i(m_textureLoc, 0);

    // 마스크 텍스처 바인딩 (텍스처 유닛 1)
    glActiveTexture(GL_TEXTURE1);
    CHECK_GL_ERROR(glActiveTexture)
    glBindTexture(GL_TEXTURE_2D, m_textureMask);
    CHECK_GL_ERROR(glBindTexture)
    glUniform1i(m_textureMaskLoc, 1);

    // 삼각형 스트립으로 사각형 그리기
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    CHECK_GL_ERROR(glDrawArrays);

    // 정리
    glDisableVertexAttribArray(m_positionLoc);
    glDisableVertexAttribArray(m_texCoordLoc);

    eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void JuneGLESLiteRtService::end()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteTextures(1, &m_texture);
    glDeleteTextures(1, &m_textureMask);
    glDeleteProgram(m_programObject);

    JuneGLESService::end();
}

void JuneGLESLiteRtService::createEGLSurface()
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
