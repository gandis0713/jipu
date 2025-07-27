#pragma once

#include <android/hardware_buffer.h>
#include <android/log.h>
#include <android/native_window.h>
#include <camera/NdkCameraCaptureSession.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraMetadata.h>
#include <functional>
#include <media/NdkImageReader.h>
#include <string>
#include <vector>

class AndroidCamera
{
public:
    enum class State
    {
        kReady = 0,
        kActive,
        kClosed,
        kError
    };

    using Callback = std::function<void(AImage* image)>;

    AndroidCamera();
    ~AndroidCamera();

    bool startCamera(const std::string& cameraId = "");
    void stopCamera();

    void setCallback(Callback callback);
    void setResolution(int width, int height);
    std::vector<std::string> getAvailableCameraIds();
    State getState() const;

private:
    bool _initialize();
    void _finalize();
    bool _createImageReader();
    bool _createCaptureRequest();
    bool _createCaptureSession();
    void _cleanupCaptureSession();
    void _cleanupImageReader();
    void _cleanupCameraDevice();

private:
    // 카메라 관련 멤버
    ACameraManager* m_cameraManager;
    ACameraDevice* m_cameraDevice;
    ACaptureRequest* m_captureRequest;
    ACameraCaptureSession* m_captureSession;
    ACaptureSessionOutput* m_sessionOutput;
    ACaptureSessionOutputContainer* m_outputContainer;
    AImageReader* m_imageReader;
    ANativeWindow* m_imageReaderWindow;

    State m_state;
    std::string m_currentCameraId;
    int m_imageWidth;
    int m_imageHeight;

    Callback m_callback;

    // 카메라 콜백 구조체들
    ACameraDevice_StateCallbacks m_deviceStateCallbacks;
    ACameraCaptureSession_stateCallbacks m_sessionStateCallbacks;
    AImageReader_ImageListener m_imageListener;
    AImageReader_BufferRemovedListener m_bufferRemovedListener;

    // 콜백 함수들
    static void onDeviceDisconnected(void* context, ACameraDevice* device);
    static void onDeviceError(void* context, ACameraDevice* device, int error);
    static void onSessionActive(void* context, ACameraCaptureSession* session);
    static void onSessionReady(void* context, ACameraCaptureSession* session);
    static void onSessionClosed(void* context, ACameraCaptureSession* session);
    static void onImageAvailable(void* context, AImageReader* reader);
    static void onBufferRemoved(void* context, AImageReader* reader, AHardwareBuffer* buffer);
};
