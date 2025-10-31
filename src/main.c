/**************************************************************************************
*           - Pixel Drawing App -                                                    
*                                                                                   
*                                                                                    
**************************************************************************************/

#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include "app_state.h"
#include "canvas.h"
#include "tools.h"
#include "ui.h"

static AppState gState = { 0 };

static void InitializePalette(AppState *state);
static void UpdateDrawFrame(void);

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib pixel art");

    InitCanvas(&gState);
    InitializePalette(&gState);
    gState.activeColorIndex = 0;
    gState.activeTool = TOOL_PENCIL;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    ShutdownCanvas(&gState);
    CloseWindow();

    return 0;
}

static void InitializePalette(AppState *state)
{
    if (!state)
    {
        return;
    }

    state->palette[0] = BLACK;
    state->palette[1] = WHITE;
    state->palette[2] = RED;
    state->palette[3] = GREEN;
    state->palette[4] = BLUE;
    state->palette[5] = YELLOW;
    state->paletteCount = 6;
}

static void UpdateDrawFrame(void)
{
    Vector2 mouse = GetMousePosition();
    bool interactingWithUI = false;

    HandlePaletteUI(&gState, mouse, &interactingWithUI);
    HandleToolUI(&gState, mouse, &interactingWithUI);
    HandleToolShortcuts(&gState);

    bool pixelModified = false;
    HandleCanvasInput(&gState, mouse, &pixelModified, interactingWithUI);
    (void)pixelModified;

    BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCanvas(&gState, mouse);
        DrawUI(&gState);
        DrawFPS(10, 10);
    EndDrawing();
}
