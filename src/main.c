#include "raylib.h"
#include "canvas.h"
#include "camera.h"
#include "tool.h"
#include "ui.h"
#include "color.h"
#include <stddef.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Global application state
static Canvas* canvas = NULL;
static CanvasCamera* camera = NULL;
static ToolState* toolState = NULL;
static ColorPicker colorPicker;
static const int pixelSize = 1; // Base pixel size before zoom

static void UpdateDrawFrame(void)
{
    // Toggle color picker with C key
    if (IsKeyPressed(KEY_C)) {
        ToggleColorPicker(&colorPicker);
        // If opening, sync with current foreground color
        if (colorPicker.isOpen && toolState != NULL) {
            SetColorPickerColor(&colorPicker, GetForegroundColor(toolState));
        }
    }

    // Handle clicking on color swatches to open picker
    Vector2 mousePos = GetMousePosition();
    Rectangle swatchArea = {10, 55, 50, 50};
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(mousePos, swatchArea) &&
        !colorPicker.isOpen) {
        ToggleColorPicker(&colorPicker);
        if (toolState != NULL) {
            SetColorPickerColor(&colorPicker, GetForegroundColor(toolState));
        }
    }

    // Update color picker (handle input and get new color)
    if (toolState != NULL) {
        Color newColor = GetForegroundColor(toolState);
        if (UpdateColorPicker(&colorPicker, &newColor)) {
            // Color changed in picker, update tool state
            SetForegroundColor(toolState, newColor);
        }
    }

    // Only update camera and tools if not interacting with color picker
    bool isOverPicker = IsMouseOverColorPicker(&colorPicker);

    // Update camera based on input (only if not over color picker)
    if (camera != NULL && !isOverPicker) {
        UpdateCanvasCamera(camera);
    }

    // Update tool state and handle drawing (only if not over color picker)
    if (toolState != NULL && canvas != NULL && camera != NULL && !isOverPicker) {
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
    DrawText("Pixel Art Tool - Color System", 10, 10, 20, WHITE);
    DrawText(TextFormat("Canvas: %dx%d pixels | Zoom: %d%% | Tool: %s",
             canvas ? canvas->width : 0,
             canvas ? canvas->height : 0,
             camera ? GetCanvasCameraZoomPercent(camera) : 100,
             toolState ? GetToolName(toolState) : "None"), 10, 35, 16, LIGHTGRAY);

    // Draw color swatches (foreground/background)
    if (toolState != NULL) {
        Color fgColor = GetForegroundColor(toolState);
        Color bgColor = GetBackgroundColor(toolState);
        DrawColorSwatches(10, 55, 40, fgColor, bgColor);
    }

    // Draw color picker UI
    if (toolState != NULL) {
        Color currentColor = GetForegroundColor(toolState);
        DrawColorPicker(&colorPicker, currentColor);
    }

    // Draw controls help text
    DrawText("Tools: B = Pencil | E = Eraser | I = Eyedropper", 10, 110, 14, GRAY);
    DrawText("Color: X = Swap | C = Color Picker | Click swatches to pick", 10, 128, 14, GRAY);
    DrawText("Pan: Middle Mouse/Space+Drag | Zoom: Mouse Wheel | R = Reset", 10, 146, 14, GRAY);

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

    // Initialize color picker (positioned on the right side of screen)
    colorPicker = InitColorPicker(screenWidth - 270, 100, 250, 250);

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
