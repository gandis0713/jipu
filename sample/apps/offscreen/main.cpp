

#include "offscreen_sample.h"

#if defined(__ANDROID__) || defined(ANDROID)

    // GameActivity's C/C++ code
    #include <game-activity/GameActivity.cpp>
    #include <game-text-input/gametextinput.cpp>

// // Glue from GameActivity to android_main()
// // Passing GameActivity event from main thread to app native thread.
extern "C"
{
    #include <game-activity/native_app_glue/android_native_app_glue.c>
}

void android_main(struct android_app* app)
{
    jipu::SampleDescriptor descriptor{
        { 1000, 2000, "Offscreen", app },
        ""
    };

    jipu::OffscreenSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    jipu::SampleDescriptor descriptor{
        { 800, 600, "Offscreen", nullptr },
        argv[0]
    };

    jipu::OffscreenSample sample(descriptor);

    return sample.exec();
}

#endif
