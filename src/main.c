#include "raylib.h"
#include "canvas.h"
#include "camera.h"
#include "tool.h"
#include <stddef.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Global application state
static Canvas* canvas = NULL;
static CanvasCamera* camera = NULL;
static ToolState* toolState = NULL;
static const int pixelSize = 1; // Base pixel size before zoom

static void UpdateDrawFrame(void)
{
    // Update camera based on input
    if (camera != NULL) {
        UpdateCanvasCamera(camera);
    }

    // Update tool state and handle drawing
    if (toolState != NULL && canvas != NULL && camera != NULL) {
        UpdateToolState(toolState, canvas, camera, pixelSize);
    }

    // Begin drawing
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // Draw the canvas
    if (canvas != NULL && camera != NULL) {
        DrawCanvas(canvas, camera->position, camera->zoom, pixelSize);

        // Draw a border around the canvas for visibility
        int scaledPixelSize = (int)(pixelSize * camera->zoom);
        int canvasScreenWidth = canvas->width * scaledPixelSize;
        int canvasScreenHeight = canvas->height * scaledPixelSize;
        DrawRectangleLines((int)camera->position.x - 1, (int)camera->position.y - 1,
                          canvasScreenWidth + 2, canvasScreenHeight + 2, WHITE);
    }

    // Draw some info text
    DrawText("Pixel Art Tool - Basic Drawing Tools", 10, 10, 20, WHITE);
    DrawText(TextFormat("Canvas: %dx%d pixels | Zoom: %d%% | Tool: %s",
             canvas ? canvas->width : 0,
             canvas ? canvas->height : 0,
             camera ? GetCanvasCameraZoomPercent(camera) : 100,
             toolState ? GetToolName(toolState) : "None"), 10, 35, 16, LIGHTGRAY);

    // Draw tool color indicator
    if (toolState != NULL) {
        Color fgColor = GetForegroundColor(toolState);
        DrawText("Color:", 10, 55, 14, GRAY);
        DrawRectangle(65, 54, 20, 16, fgColor);
        DrawRectangleLines(65, 54, 20, 16, WHITE);
    }

    // Draw controls help text
    DrawText("Controls: Left Click to Draw | B = Pencil, E = Eraser", 10, 75, 14, GRAY);
    DrawText("Pan: Middle Mouse/Space+Drag | Zoom: Mouse Wheel | R = Reset", 10, 93, 14, GRAY);

    EndDrawing();
}

int main(void)
{
    const int screenWidth = 1024;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "Pixel Art Tool");
    SetTargetFPS(60);

    // Create a 64x64 pixel canvas
    canvas = CreateCanvas(64, 64);
    if (!canvas) {
        TraceLog(LOG_ERROR, "Failed to create canvas");
        CloseWindow();
        return 1;
    }

    // Create the camera
    camera = CreateCanvasCamera();
    if (!camera) {
        TraceLog(LOG_ERROR, "Failed to create camera");
        DestroyCanvas(canvas);
        CloseWindow();
        return 1;
    }

    // Create the tool state
    toolState = CreateToolState();
    if (!toolState) {
        TraceLog(LOG_ERROR, "Failed to create tool state");
        DestroyCanvasCamera(camera);
        DestroyCanvas(canvas);
        CloseWindow();
        return 1;
    }

    // Center the canvas on screen
    int scaledPixelSize = (int)(pixelSize * camera->zoom);
    camera->position.x = (screenWidth - (canvas->width * scaledPixelSize)) / 2.0f;
    camera->position.y = (screenHeight - (canvas->height * scaledPixelSize)) / 2.0f;

    // Canvas starts empty - ready for user to draw!


#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    // Cleanup
    DestroyToolState(toolState);
    DestroyCanvasCamera(camera);
    DestroyCanvas(canvas);
    CloseWindow();

    return 0;
}
