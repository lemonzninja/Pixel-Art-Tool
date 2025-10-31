#include "app.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

static App gApp;

static void UpdateDrawFrame(void)
{
    AppUpdate(&gApp);
    AppDraw(&gApp);
}

int main(void)
{
    const int screenWidth = 1024;
    const int screenHeight = 768;

    if (!AppInit(&gApp, screenWidth, screenHeight))
    {
        return 1;
    }

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    AppShutdown(&gApp);
    return 0;
}
