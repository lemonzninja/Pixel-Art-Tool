#include "canvas.h"
#include <stdlib.h>
#include <string.h>

// Create a new canvas with specified dimensions
Canvas* CreateCanvas(int width, int height) {
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    Canvas* canvas = (Canvas*)malloc(sizeof(Canvas));
    if (!canvas) {
        return NULL;
    }

    canvas->width = width;
    canvas->height = height;
    canvas->pixels = (Color*)malloc(sizeof(Color) * width * height);

    if (!canvas->pixels) {
        free(canvas);
        return NULL;
    }

    // Initialize all pixels to transparent white
    ClearCanvas(canvas, (Color){255, 255, 255, 0});

    return canvas;
}

// Free canvas memory
void DestroyCanvas(Canvas* canvas) {
    if (canvas) {
        if (canvas->pixels) {
            free(canvas->pixels);
        }
        free(canvas);
    }
}

// Clear the entire canvas with a color
void ClearCanvas(Canvas* canvas, Color color) {
    if (!canvas || !canvas->pixels) {
        return;
    }

    for (int i = 0; i < canvas->width * canvas->height; i++) {
        canvas->pixels[i] = color;
    }
}

// Set a pixel at the given coordinates
void SetPixel(Canvas* canvas, int x, int y, Color color) {
    if (!IsValidPixelCoord(canvas, x, y)) {
        return;
    }

    int index = y * canvas->width + x;
    canvas->pixels[index] = color;
}

// Get a pixel color at the given coordinates
Color GetPixel(Canvas* canvas, int x, int y) {
    if (!IsValidPixelCoord(canvas, x, y)) {
        return (Color){0, 0, 0, 0};
    }

    int index = y * canvas->width + x;
    return canvas->pixels[index];
}

// Check if pixel coordinates are within canvas bounds
bool IsValidPixelCoord(Canvas* canvas, int x, int y) {
    if (!canvas) {
        return false;
    }
    return (x >= 0 && x < canvas->width && y >= 0 && y < canvas->height);
}

// Convert pixel coordinates to screen coordinates
Vector2 PixelToScreen(int pixelX, int pixelY, Vector2 canvasOffset, float zoom, int pixelSize) {
    Vector2 screenPos;
    screenPos.x = canvasOffset.x + (pixelX * pixelSize * zoom);
    screenPos.y = canvasOffset.y + (pixelY * pixelSize * zoom);
    return screenPos;
}

// Convert screen coordinates to pixel coordinates
Vector2 ScreenToPixel(int screenX, int screenY, Vector2 canvasOffset, float zoom, int pixelSize) {
    Vector2 pixelPos;
    pixelPos.x = (screenX - canvasOffset.x) / (pixelSize * zoom);
    pixelPos.y = (screenY - canvasOffset.y) / (pixelSize * zoom);
    return pixelPos;
}

// Draw checkerboard pattern for transparency background
void DrawCheckerboardBackground(int x, int y, int width, int height, int pixelSize, float zoom) {
    const int checkerSize = 8; // Size of checker squares in pixels
    const Color lightGray = (Color){200, 200, 200, 255};
    const Color darkGray = (Color){170, 170, 170, 255};

    int scaledPixelSize = (int)(pixelSize * zoom);

    // Calculate how many checker squares fit in the canvas
    int checkersWide = (width + checkerSize - 1) / checkerSize;
    int checkersHigh = (height + checkerSize - 1) / checkerSize;

    for (int cy = 0; cy < checkersHigh; cy++) {
        for (int cx = 0; cx < checkersWide; cx++) {
            // Determine checker color (alternating pattern)
            Color checkerColor = ((cx + cy) % 2 == 0) ? lightGray : darkGray;

            // Calculate screen position and size for this checker square
            int checkerPixelX = cx * checkerSize;
            int checkerPixelY = cy * checkerSize;

            int screenX = x + (int)(checkerPixelX * scaledPixelSize);
            int screenY = y + (int)(checkerPixelY * scaledPixelSize);

            // Calculate actual size (handle edge cases where checker extends beyond canvas)
            int checkerWidthPixels = checkerSize;
            int checkerHeightPixels = checkerSize;

            if (checkerPixelX + checkerSize > width) {
                checkerWidthPixels = width - checkerPixelX;
            }
            if (checkerPixelY + checkerSize > height) {
                checkerHeightPixels = height - checkerPixelY;
            }

            int screenWidth = checkerWidthPixels * scaledPixelSize;
            int screenHeight = checkerHeightPixels * scaledPixelSize;

            DrawRectangle(screenX, screenY, screenWidth, screenHeight, checkerColor);
        }
    }
}

// Draw the canvas to the screen
void DrawCanvas(Canvas* canvas, Vector2 offset, float zoom, int pixelSize) {
    if (!canvas || !canvas->pixels) {
        return;
    }

    int scaledPixelSize = (int)(pixelSize * zoom);

    // First, draw the checkerboard background
    DrawCheckerboardBackground((int)offset.x, (int)offset.y,
                              canvas->width, canvas->height,
                              pixelSize, zoom);

    // Then draw each pixel
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            Color pixelColor = GetPixel(canvas, x, y);

            // Only draw non-fully-transparent pixels
            if (pixelColor.a > 0) {
                Vector2 screenPos = PixelToScreen(x, y, offset, zoom, pixelSize);
                DrawRectangle((int)screenPos.x, (int)screenPos.y,
                            scaledPixelSize, scaledPixelSize, pixelColor);
            }
        }
    }
}
