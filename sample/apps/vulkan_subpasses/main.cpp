#include "vulkan_subpasses_sample.h"

#include "hpc_watcher.h"
#include "hpc/hpc.h"

#include <spdlog/spdlog.h>

 #if defined(__ANDROID__) || defined(ANDROID)

// GameActivity's C/C++ code
#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

// Glue from GameActivity to android_main()
// Passing GameActivity event from main thread to app native thread.
extern "C"
{
#include <game-activity/native_app_glue/android_native_app_glue.c>
}

void android_main(struct android_app* app)
{
    jipu::SampleDescriptor descriptor{
        { 1000, 2000, "Vulkan Subpasses Sample", app },
        ""
    };

    jipu::VulkanSubpassesSample sample(descriptor);

    auto gpus = jipu::hpc::gpus();
    if (!gpus.empty())
    {
        auto gpu = gpus[0].get();
        jipu::hpc::Counter::Ptr counter = gpu->create();
        jipu::hpc::Sampler::Ptr sampler = counter->create();

        jipu::HPCWatcher::Ptr watcher = std::make_unique<jipu::HPCWatcher>(std::move(sampler));

        sample.setHPCWatcher(std::move(watcher));
    }

    sample.exec();
}

 #else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    jipu::SampleDescriptor descriptor{
        { 800, 600, "Vulkan Subpasses Sample", nullptr },
        argv[0]
    };

    jipu::VulkanSubpassesSample sample(descriptor);

    return sample.exec();
}

 #endif