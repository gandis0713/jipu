#include <android/log.h>
#include <jni.h>
#include <memory>
#include <spdlog/spdlog.h>

extern "C" JNIEXPORT void JNICALL
Java_com_gandis_jipu_MainActivity_nativeInitializeCamera(JNIEnv* env, jobject thiz)
{
    spdlog::info("JNI: Initializing camera");
}
