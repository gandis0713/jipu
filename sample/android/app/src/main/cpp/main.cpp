//
// Created by user on 2023/05/31.
//

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

//// Glue from GameActivity to android_main()
//// Passing GameActivity event from main thread to app native thread.
extern  "C" {
#include <game-activity/native_app_glue/android_native_app_glue.c>
}

void handle_cmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            __android_log_print(ANDROID_LOG_INFO, "Vulkan Test",
                                "event handled: %d", cmd);
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            __android_log_print(ANDROID_LOG_INFO, "Vulkan Test",
                                "event handled: %d", cmd);
            break;
        default:
            __android_log_print(ANDROID_LOG_INFO, "Vulkan Test",
                                "event not handled: %d", cmd);
    }
}

void android_main(struct android_app* app)
{
    // Set the callback to process system events
    app->onAppCmd = handle_cmd;

    // Used to poll the events in the main loop
    int events;
    android_poll_source* source;

    // Main loop
    do {
        if (ALooper_pollAll(0, nullptr,
                            &events, (void**)&source) >= 0) {
            if (source != NULL) source->process(app, source);
        }

    } while (app->destroyRequested == 0);
}

