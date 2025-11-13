#ifndef CANVAS_H
#define CANVAS_H

#include "raylib.h"
#include <stdbool.h>

// Canvas structure for pixel data storage
typedef struct {
    int width;              // Canvas width in pixels
    int height;             // Canvas height in pixels
    Color* pixels;          // 2D array stored as 1D (width * height)
} Canvas;

// Canvas initialization and cleanup
Canvas* CreateCanvas(int width, int height);
void DestroyCanvas(Canvas* canvas);
void ClearCanvas(Canvas* canvas, Color color);

// Pixel operations
void SetPixel(Canvas* canvas, int x, int y, Color color);
Color GetPixel(Canvas* canvas, int x, int y);
bool IsValidPixelCoord(Canvas* canvas, int x, int y);

// Coordinate conversion
Vector2 PixelToScreen(int pixelX, int pixelY, Vector2 canvasOffset, float zoom, int pixelSize);
Vector2 ScreenToPixel(int screenX, int screenY, Vector2 canvasOffset, float zoom, int pixelSize);

// Rendering
void DrawCanvas(Canvas* canvas, Vector2 offset, float zoom, int pixelSize);
void DrawCheckerboardBackground(int x, int y, int width, int height, int pixelSize, float zoom);

#endif // CANVAS_H
