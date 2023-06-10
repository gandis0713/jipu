#include "window.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>

// GameActivity's C/C++ code
#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

// Glue from GameActivity to android_main()
// Passing GameActivity event from main thread to app native thread.
extern "C"
{
#include <game-activity/native_app_glue/android_native_app_glue.c>
}

// Process the next main command.

Window::Window(const WindowDescriptor& descriptor, void* handle)
    : m_handle(handle)
{
    struct android_app* app = static_cast<android_app*>(m_handle);

    auto onAppCmd = [this](android_app* app, int32_t cmd)
    {
        m_handle = app;

        switch (cmd)
        {
        case APP_CMD_INIT_WINDOW:
            // TODO: init VKT
            break;
        case APP_CMD_TERM_WINDOW:
            // TODO: delete VKT
            break;
        default:
            break;
        }
    };

    // Set the callback to process system events
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
        if (ALooper_pollAll(1, nullptr, &events, (void**)&source) >= 0)
        {
            if (source != NULL)
                source->process(app, source);
        }

        draw();

    } while (app->destroyRequested == 0);

    return 0;
}