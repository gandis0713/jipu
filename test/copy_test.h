#pragma once

#if defined(OFFSCREEN_TEST)
    #include "base/offscreen_test.h"
#else
    #include "base/window_test.h"
#endif

#if defined(OFFSCREEN_TEST)
class CopyTest : public OffscreenTest
#else
class CopyTest : public WindowTest
#endif
{
};