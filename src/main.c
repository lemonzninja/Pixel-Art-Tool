#include "app.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

static App gApp;

static void UpdateDrawFrame(void)
{

}

int main(void)
{
    const int screenWidth = 1024;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "Pixel art tooL");
    
    
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    return 0;
}
