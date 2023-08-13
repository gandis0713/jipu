
#include "sample.h"

#include <spdlog/spdlog.h>

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

#endif

namespace vkt
{

class ComputeSample : public Sample
{
public:
    ComputeSample() = delete;
    ComputeSample(const SampleDescriptor& descriptor);
    ~ComputeSample() override;

public:
    void init() override;
    void draw() override;
};

ComputeSample::ComputeSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

ComputeSample::~ComputeSample()
{
}

void ComputeSample::init()
{
}

void ComputeSample::draw()
{
}

} // namespace vkt

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Compute Shader Sample", app },
        ""
    };

    vkt::ComputeSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Compute Shader Sample", nullptr },
        argv[0]
    };

    vkt::ComputeSample sample(descriptor);

    return sample.exec();
}

#endif
