/**
 * tool.c
 *
 * Implementation of Drawing Tool System
 */

#include "tool.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

/**
 * Create and initialize a new tool state
 */
ToolState* CreateToolState(void) {
    ToolState* state = (ToolState*)malloc(sizeof(ToolState));
    if (state == NULL) {
        return NULL;
    }

    // Initialize with defaults
    state->currentTool = TOOL_PENCIL;
    state->foregroundColor = BLACK;
    state->backgroundColor = WHITE;
    state->isDrawing = false;
    state->lastPixelX = 0;
    state->lastPixelY = 0;
    state->hasLastPixel = false;

    return state;
}

/**
 * Destroy tool state and free memory
 */
void DestroyToolState(ToolState* state) {
    if (state != NULL) {
        free(state);
    }
}

/**
 * Set the current drawing tool
 */
void SetCurrentTool(ToolState* state, ToolType tool) {
    if (state == NULL) return;
    state->currentTool = tool;
}

/**
 * Get the current drawing tool
 */
ToolType GetCurrentTool(ToolState* state) {
    if (state == NULL) return TOOL_PENCIL;
    return state->currentTool;
}

/**
 * Set the foreground color
 */
void SetForegroundColor(ToolState* state, Color color) {
    if (state == NULL) return;
    state->foregroundColor = color;
}

/**
 * Set the background color
 */
void SetBackgroundColor(ToolState* state, Color color) {
    if (state == NULL) return;
    state->backgroundColor = color;
}

/**
 * Get the current foreground color
 */
Color GetForegroundColor(ToolState* state) {
    if (state == NULL) return BLACK;
    return state->foregroundColor;
}

/**
 * Get the current background color
 */
Color GetBackgroundColor(ToolState* state) {
    if (state == NULL) return WHITE;
    return state->backgroundColor;
}

/**
 * Draw a single pixel with the current tool at the given canvas coordinates
 */
void DrawPixelWithTool(ToolState* state, Canvas* canvas, int pixelX, int pixelY) {
    if (state == NULL || canvas == NULL) return;

    // Check if coordinates are valid
    if (!IsValidPixelCoord(canvas, pixelX, pixelY)) {
        return;
    }

    // Apply tool effect
    switch (state->currentTool) {
        case TOOL_PENCIL:
            // Draw with foreground color
            SetPixel(canvas, pixelX, pixelY, state->foregroundColor);
            break;

        case TOOL_ERASER:
            // Erase by setting to transparent
            SetPixel(canvas, pixelX, pixelY, (Color){0, 0, 0, 0});
            break;

        default:
            break;
    }
}

/**
 * Draw a line between two points using Bresenham's line algorithm
 * This is used for smooth drag drawing
 */
static void DrawLineBetweenPixels(ToolState* state, Canvas* canvas, int x0, int y0, int x1, int y1) {
    // Bresenham's line algorithm
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int x = x0;
    int y = y0;

    while (true) {
        // Draw pixel at current position
        DrawPixelWithTool(state, canvas, x, y);

        // Check if we've reached the end
        if (x == x1 && y == y1) {
            break;
        }

        // Calculate error and step
        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }

        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

/**
 * Update tool state based on user input
 */
void UpdateToolState(ToolState* state, Canvas* canvas, CanvasCamera* camera, int pixelSize) {
    if (state == NULL || canvas == NULL || camera == NULL) return;

    // --- Handle Tool Switching with Keyboard Shortcuts ---

    if (IsKeyPressed(KEY_B)) {
        SetCurrentTool(state, TOOL_PENCIL);
    }

    if (IsKeyPressed(KEY_E)) {
        SetCurrentTool(state, TOOL_ERASER);
    }

    // --- Handle Drawing Input ---

    // Don't draw if the camera is panning
    // Camera pans with middle mouse or spacebar + left mouse
    bool isPanning = camera->isPanning;
    bool spacebarDown = IsKeyDown(KEY_SPACE);

    // Only allow drawing with left mouse if not panning
    bool canDraw = !isPanning && !spacebarDown;

    if (canDraw) {
        // Check if left mouse button is pressed or held
        bool leftMousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool leftMouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        if (leftMousePressed) {
            // Start drawing
            state->isDrawing = true;
            state->hasLastPixel = false; // Reset for new stroke

            // Get mouse position and convert to canvas coordinates
            Vector2 mousePos = GetMousePosition();
            Vector2 pixelPos = ScreenToPixel((int)mousePos.x, (int)mousePos.y,
                                            camera->position, camera->zoom, pixelSize);

            int pixelX = (int)floor(pixelPos.x);
            int pixelY = (int)floor(pixelPos.y);

            // Draw the pixel
            DrawPixelWithTool(state, canvas, pixelX, pixelY);

            // Store last pixel position for drag drawing
            state->lastPixelX = pixelX;
            state->lastPixelY = pixelY;
            state->hasLastPixel = true;

        } else if (leftMouseDown && state->isDrawing) {
            // Continue drawing (drag)
            Vector2 mousePos = GetMousePosition();
            Vector2 pixelPos = ScreenToPixel((int)mousePos.x, (int)mousePos.y,
                                            camera->position, camera->zoom, pixelSize);

            int pixelX = (int)floor(pixelPos.x);
            int pixelY = (int)floor(pixelPos.y);

            // Only draw if we've moved to a different pixel
            if (state->hasLastPixel &&
                (pixelX != state->lastPixelX || pixelY != state->lastPixelY)) {
                // Draw a line from last pixel to current pixel for smooth drawing
                DrawLineBetweenPixels(state, canvas,
                                    state->lastPixelX, state->lastPixelY,
                                    pixelX, pixelY);

                // Update last pixel position
                state->lastPixelX = pixelX;
                state->lastPixelY = pixelY;
            } else if (!state->hasLastPixel) {
                // First pixel in drag (shouldn't happen, but handle it)
                DrawPixelWithTool(state, canvas, pixelX, pixelY);
                state->lastPixelX = pixelX;
                state->lastPixelY = pixelY;
                state->hasLastPixel = true;
            }

        } else {
            // Mouse released, stop drawing
            state->isDrawing = false;
            state->hasLastPixel = false;
        }
    } else {
        // Can't draw while panning, stop drawing state
        if (state->isDrawing) {
            state->isDrawing = false;
            state->hasLastPixel = false;
        }
    }
}

/**
 * Get the name of the current tool as a string
 */
const char* GetToolName(ToolState* state) {
    if (state == NULL) return "Unknown";

    switch (state->currentTool) {
        case TOOL_PENCIL:
            return "Pencil";
        case TOOL_ERASER:
            return "Eraser";
        default:
            return "Unknown";
    }
}
