#include "window.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>

// // GameActivity's C/C++ code
// #include <game-activity/GameActivity.cpp>
// #include <game-text-input/gametextinput.cpp>

// // // Glue from GameActivity to android_main()
// // // Passing GameActivity event from main thread to app native thread.
// extern "C"
// {
// #include <game-activity/native_app_glue/android_native_app_glue.c>
// }

namespace vkt
{

static void onAppCmd(android_app* app, int32_t cmd)
{
    Window* window = static_cast<Window*>(app->userData);
    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
        // TODO: init VKT
        window->init();
        break;
    case APP_CMD_TERM_WINDOW:
        // TODO: delete VKT
        break;
    default:
        break;
    }
}

Window::Window(const WindowDescriptor& descriptor)
    : m_handle(descriptor.handle)
{
    struct android_app* app = static_cast<android_app*>(m_handle);

    // Set the callback to process system events
    app->userData = this;
    app->onAppCmd = onAppCmd;
}

Window::~Window()
{
    // do nothing.
}

int Window::exec()
{
    android_app* app = static_cast<android_app*>(m_handle);

    int events;
    android_poll_source* source;

    // Main loop
    do
    {
        if (ALooper_pollAll(isInitialized(), nullptr, &events, (void**)&source) >= 0)
        {
            if (source != NULL)
                source->process(app, source);
        }

        if(isInitialized())
        {
            draw();
        }

    } while (app->destroyRequested == 0);

    return 0;
}

void* Window::getWindowHandle()
{
    struct android_app* app = static_cast<android_app*>(m_handle);
    return static_cast<void*>(app->window);
}

} // namespace vkt