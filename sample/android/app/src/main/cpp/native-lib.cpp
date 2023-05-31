#include <jni.h>
#include <string>
#include "VKT.h"


extern "C"
{

JNIEXPORT jstring JNICALL
Java_com_gandis_vulkan_1test_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jstring JNICALL
Java_com_gandis_vulkan_1test_MainActivity_stringFromJNI2(
        JNIEnv *env,
jobject /* this */) {
    VKT vkt;
std::string hello = vkt.getName();
return env->NewStringUTF(hello.c_str());
}

}