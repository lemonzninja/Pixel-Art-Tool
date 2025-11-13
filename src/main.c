#include "raylib.h"
#include "canvas.h"
#include "camera.h"
#include <stddef.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Global application state
static Canvas* canvas = NULL;
static CanvasCamera* camera = NULL;
static const int pixelSize = 1; // Base pixel size before zoom

static void UpdateDrawFrame(void)
{
    // Update camera based on input
    if (camera != NULL) {
        UpdateCanvasCamera(camera);
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
    DrawText("Pixel Art Tool - Camera/Viewport System", 10, 10, 20, WHITE);
    DrawText(TextFormat("Canvas: %dx%d pixels | Zoom: %d%%",
             canvas ? canvas->width : 0,
             canvas ? canvas->height : 0,
             camera ? GetCanvasCameraZoomPercent(camera) : 100), 10, 35, 16, LIGHTGRAY);

    // Draw camera controls help text
    DrawText("Controls: Middle Mouse/Space+Drag to Pan | Mouse Wheel to Zoom | R to Reset", 10, 55, 14, GRAY);

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

    // Center the canvas on screen
    int scaledPixelSize = (int)(pixelSize * camera->zoom);
    camera->position.x = (screenWidth - (canvas->width * scaledPixelSize)) / 2.0f;
    camera->position.y = (screenHeight - (canvas->height * scaledPixelSize)) / 2.0f;

    // Draw some sample pixels to demonstrate the canvas
    // Draw a simple pattern: border and diagonal lines
    for (int x = 0; x < canvas->width; x++) {
        SetPixel(canvas, x, 0, RED);                    // Top border
        SetPixel(canvas, x, canvas->height - 1, RED);   // Bottom border
    }
    for (int y = 0; y < canvas->height; y++) {
        SetPixel(canvas, 0, y, RED);                    // Left border
        SetPixel(canvas, canvas->width - 1, y, RED);    // Right border
    }
    // Diagonal lines
    for (int i = 0; i < canvas->width && i < canvas->height; i++) {
        SetPixel(canvas, i, i, BLUE);                           // Top-left to bottom-right
        SetPixel(canvas, canvas->width - 1 - i, i, GREEN);      // Top-right to bottom-left
    }
    // Center cross
    int centerX = canvas->width / 2;
    int centerY = canvas->height / 2;
    for (int i = 0; i < canvas->width; i++) {
        SetPixel(canvas, i, centerY, YELLOW);
    }
    for (int i = 0; i < canvas->height; i++) {
        SetPixel(canvas, centerX, i, YELLOW);
    }


#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    // Cleanup
    DestroyCanvasCamera(camera);
    DestroyCanvas(canvas);
    CloseWindow();

    return 0;
}
