
#include "android_camera.h"

#include <spdlog/spdlog.h>

AndroidCamera::AndroidCamera()
    : m_cameraManager(nullptr)
    , m_cameraDevice(nullptr)
    , m_captureRequest(nullptr)
    , m_captureSession(nullptr)
    , m_sessionOutput(nullptr)
    , m_outputContainer(nullptr)
    , m_imageReader(nullptr)
    , m_imageReaderWindow(nullptr)
    , m_state(State::kReady)
    , m_imageWidth(640)
    , m_imageHeight(480)
{
    _initialize();
}

// 소멸자
AndroidCamera::~AndroidCamera()
{
    _finalize();
}

// 카메라 시작
bool AndroidCamera::startCamera(const std::string& cameraId)
{
    if (m_state != State::kReady)
    {
        spdlog::error("Camera not in ready state");
        return false;
    }

    // 카메라 ID 결정
    std::string targetCameraId = cameraId;
    if (targetCameraId.empty())
    {
        auto cameraIds = getAvailableCameraIds();
        if (cameraIds.empty())
        {
            spdlog::error("No cameras available");
            return false;
        }
        targetCameraId = cameraIds[0];
    }

    m_currentCameraId = targetCameraId;

    // 카메라 디바이스 열기
    camera_status_t status = ACameraManager_openCamera(m_cameraManager,
                                                       m_currentCameraId.c_str(),
                                                       &m_deviceStateCallbacks,
                                                       &m_cameraDevice);
    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to open camera: {}", m_currentCameraId.c_str());
        return false;
    }

    // 이미지 리더 생성
    if (!_createImageReader())
    {
        _cleanupCameraDevice();
        return false;
    }

    // 캡처 요청 생성
    if (!_createCaptureRequest())
    {
        _cleanupImageReader();
        _cleanupCameraDevice();
        return false;
    }

    // 캡처 세션 생성
    if (!_createCaptureSession())
    {
        _cleanupImageReader();
        _cleanupCameraDevice();
        return false;
    }

    // 반복 캡처 시작
    status = ACameraCaptureSession_setRepeatingRequest(m_captureSession, nullptr, 1,
                                                       &m_captureRequest, nullptr);
    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to start repeating request");
        _cleanupCaptureSession();
        _cleanupImageReader();
        _cleanupCameraDevice();
        return false;
    }

    m_state = State::kActive;
    spdlog::info("Camera started successfully: {}", m_currentCameraId.c_str());
    return true;
}

// 카메라 중지
void AndroidCamera::stopCamera()
{
    if (m_state != State::kActive)
    {
        return;
    }

    _cleanupCaptureSession();
    _cleanupImageReader();
    _cleanupCameraDevice();

    m_state = State::kReady;
    spdlog::info("Camera stopped");
}

// 해상도 설정
void AndroidCamera::setResolution(int width, int height)
{
    m_imageWidth = width;
    m_imageHeight = height;
}

void AndroidCamera::setCallback(Callback callback)
{
    m_callback = callback;
}

AndroidCamera::State AndroidCamera::getState() const
{
    return m_state;
}

// 사용 가능한 카메라 ID 목록 가져오기
std::vector<std::string> AndroidCamera::getAvailableCameraIds()
{
    std::vector<std::string> cameraIds;

    if (m_cameraManager == nullptr)
    {
        spdlog::error("Camera manager not initialized");
        return cameraIds;
    }

    ACameraIdList* cameraIdList = nullptr;
    camera_status_t status = ACameraManager_getCameraIdList(m_cameraManager, &cameraIdList);

    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to get camera ID list");
        return cameraIds;
    }

    for (int i = 0; i < cameraIdList->numCameras; i++)
    {
        cameraIds.push_back(std::string(cameraIdList->cameraIds[i]));
    }

    ACameraManager_deleteCameraIdList(cameraIdList);
    return cameraIds;
}

// 이미지 리더 생성
bool AndroidCamera::_createImageReader()
{
    //    uint64_t usage = AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN ;
    uint64_t usage = AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_CPU_READ_NEVER;

    media_status_t status = AImageReader_newWithUsage(m_imageWidth, m_imageHeight,
                                                      AIMAGE_FORMAT_YUV_420_888, usage, 2, &m_imageReader);
    if (status != AMEDIA_OK)
    {
        spdlog::error("Failed to create image reader");
        return false;
    }

    status = AImageReader_getWindow(m_imageReader, &m_imageReaderWindow);
    if (status != AMEDIA_OK)
    {
        spdlog::error("Failed to get image reader window");
        return false;
    }

    status = AImageReader_setImageListener(m_imageReader, &m_imageListener);
    if (status != AMEDIA_OK)
    {
        spdlog::error("Failed to set image listener");
        return false;
    }

    status = AImageReader_setBufferRemovedListener(m_imageReader, &m_bufferRemovedListener);
    if (status != AMEDIA_OK)
    {
        spdlog::error("Failed to set buffer removed listener");
        return false;
    }

    return true;
}

// 캡처 요청 생성
bool AndroidCamera::_createCaptureRequest()
{
    camera_status_t status = ACameraDevice_createCaptureRequest(m_cameraDevice,
                                                                TEMPLATE_PREVIEW, &m_captureRequest);
    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to create capture request");
        return false;
    }

    // ANativeWindow를 ACameraOutputTarget으로 변환
    ACameraOutputTarget* outputTarget = nullptr;
    status = ACameraOutputTarget_create(m_imageReaderWindow, &outputTarget);
    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to create camera output target");
        return false;
    }

    // 캡처 요청에 타겟 추가
    status = ACaptureRequest_addTarget(m_captureRequest, outputTarget);
    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to add target to capture request");
        ACameraOutputTarget_free(outputTarget);
        return false;
    }

    // 타겟은 캡처 요청에 추가된 후 해제 가능
    ACameraOutputTarget_free(outputTarget);

    return true;
}

// 캡처 세션 생성
bool AndroidCamera::_createCaptureSession()
{
    camera_status_t status = ACaptureSessionOutputContainer_create(&m_outputContainer);
    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to create output container");
        return false;
    }

    status = ACaptureSessionOutput_create(m_imageReaderWindow, &m_sessionOutput);
    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to create session output");
        return false;
    }

    status = ACaptureSessionOutputContainer_add(m_outputContainer, m_sessionOutput);
    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to add output to container");
        return false;
    }

    status = ACameraDevice_createCaptureSession(m_cameraDevice, m_outputContainer,
                                                &m_sessionStateCallbacks, &m_captureSession);
    if (status != ACAMERA_OK)
    {
        spdlog::error("Failed to create capture session");
        return false;
    }

    return true;
}

void AndroidCamera::_cleanupCaptureSession()
{
    if (m_captureSession)
    {
        ACameraCaptureSession_close(m_captureSession);
        m_captureSession = nullptr;
    }

    if (m_captureRequest)
    {
        ACaptureRequest_free(m_captureRequest);
        m_captureRequest = nullptr;
    }

    if (m_sessionOutput)
    {
        ACaptureSessionOutput_free(m_sessionOutput);
        m_sessionOutput = nullptr;
    }

    if (m_outputContainer)
    {
        ACaptureSessionOutputContainer_free(m_outputContainer);
        m_outputContainer = nullptr;
    }
}

void AndroidCamera::_cleanupImageReader()
{
    if (m_imageReader)
    {
        AImageReader_delete(m_imageReader);
        m_imageReader = nullptr;
        m_imageReaderWindow = nullptr;
    }
}

void AndroidCamera::_cleanupCameraDevice()
{
    if (m_cameraDevice)
    {
        ACameraDevice_close(m_cameraDevice);
        m_cameraDevice = nullptr;
    }
}

bool AndroidCamera::_initialize()
{
    {
        m_deviceStateCallbacks.onDisconnected = onDeviceDisconnected;
        m_deviceStateCallbacks.onError = onDeviceError;
        m_deviceStateCallbacks.context = this;

        m_sessionStateCallbacks.onActive = onSessionActive;
        m_sessionStateCallbacks.onReady = onSessionReady;
        m_sessionStateCallbacks.onClosed = onSessionClosed;
        m_sessionStateCallbacks.context = this;

        m_imageListener.onImageAvailable = onImageAvailable;
        m_imageListener.context = this;

        m_bufferRemovedListener.onBufferRemoved = onBufferRemoved;
        m_bufferRemovedListener.context = this;
    }

    m_cameraManager = ACameraManager_create();
    if (m_cameraManager == nullptr)
    {
        spdlog::error("Failed to create camera manager");
        return false;
    }

    spdlog::info("Camera manager created successfully");
    return true;
}

void AndroidCamera::_finalize()
{
    stopCamera();

    if (m_cameraManager)
    {
        ACameraManager_delete(m_cameraManager);
        m_cameraManager = nullptr;
    }

    m_state = State::kClosed;
    spdlog::info("Camera resources released");
}

// 콜백 함수 구현
void AndroidCamera::onDeviceDisconnected(void* context, ACameraDevice* device)
{
    spdlog::info("Camera device disconnected");
    AndroidCamera* camera = static_cast<AndroidCamera*>(context);
    camera->m_state = State::kError;
}

void AndroidCamera::onDeviceError(void* context, ACameraDevice* device, int error)
{
    spdlog::error("Camera device error: {}", error);
    AndroidCamera* camera = static_cast<AndroidCamera*>(context);
    camera->m_state = State::kError;
}

void AndroidCamera::onSessionActive(void* context, ACameraCaptureSession* session)
{
    spdlog::info("Capture session active");
}

void AndroidCamera::onSessionReady(void* context, ACameraCaptureSession* session)
{
    spdlog::info("Capture session ready");
}

void AndroidCamera::onSessionClosed(void* context, ACameraCaptureSession* session)
{
    spdlog::info("Capture session closed");
}

void AndroidCamera::onImageAvailable(void* context, AImageReader* reader)
{
    AndroidCamera* camera = static_cast<AndroidCamera*>(context);

    AImage* image = nullptr;
    media_status_t status = AImageReader_acquireLatestImage(reader, &image);

    if (status != AMEDIA_OK)
    {
        spdlog::error("Failed to acquire image");
        return;
    }

    camera->m_callback(image);

    AImage_delete(image);
}

void AndroidCamera::onBufferRemoved(void* context, AImageReader* reader, AHardwareBuffer* buffer)
{
    spdlog::info("HardwareBuffer removed from ImageReader");
}