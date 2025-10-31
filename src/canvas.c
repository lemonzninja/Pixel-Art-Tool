#include "canvas.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

bool CanvasInit(Canvas *canvas, int width, int height, int pixelSize, Color initialColor, Color backgroundColor)
{
    if (!canvas || width <= 0 || height <= 0 || pixelSize <= 0) return false;

    const size_t pixelCount = (size_t)width * (size_t)height;
    Color *pixels = (Color *)MemAlloc(pixelCount * sizeof(Color));
    if (!pixels) return false;

    canvas->width = width;
    canvas->height = height;
    canvas->pixelSize = pixelSize;
    canvas->pixels = pixels;
    canvas->backgroundColor = backgroundColor;

    CanvasClear(canvas, initialColor);
    return true;
}

void CanvasUnload(Canvas *canvas)
{
    if (!canvas || !canvas->pixels) return;
    MemFree(canvas->pixels);
    canvas->pixels = NULL;
    canvas->width = 0;
    canvas->height = 0;
    canvas->pixelSize = 0;
}

void CanvasClear(Canvas *canvas, Color color)
{
    if (!canvas || !canvas->pixels) return;
    const size_t pixelCount = (size_t)canvas->width * (size_t)canvas->height;
    for (size_t i = 0; i < pixelCount; ++i)
    {
        canvas->pixels[i] = color;
    }
}

bool CanvasContains(const Canvas *canvas, int x, int y)
{
    if (!canvas) return false;
    return x >= 0 && y >= 0 && x < canvas->width && y < canvas->height;
}

void CanvasSetPixel(Canvas *canvas, int x, int y, Color color)
{
    if (!canvas || !canvas->pixels) return;
    if (!CanvasContains(canvas, x, y)) return;
    canvas->pixels[y * canvas->width + x] = color;
}

Color CanvasGetPixel(const Canvas *canvas, int x, int y)
{
    if (!canvas || !canvas->pixels) return BLANK;
    if (!CanvasContains(canvas, x, y)) return BLANK;
    return canvas->pixels[y * canvas->width + x];
}

bool CanvasWorldToPixel(const Canvas *canvas, Vector2 worldPosition, int *outX, int *outY)
{
    if (!canvas || !outX || !outY) return false;
    if (worldPosition.x < 0.0f || worldPosition.y < 0.0f) return false;

    int x = (int)floorf(worldPosition.x / (float)canvas->pixelSize);
    int y = (int)floorf(worldPosition.y / (float)canvas->pixelSize);

    if (!CanvasContains(canvas, x, y)) return false;

    *outX = x;
    *outY = y;
    return true;
}

float CanvasGetWidthPixels(const Canvas *canvas)
{
    if (!canvas) return 0.0f;
    return (float)(canvas->width * canvas->pixelSize);
}

float CanvasGetHeightPixels(const Canvas *canvas)
{
    if (!canvas) return 0.0f;
    return (float)(canvas->height * canvas->pixelSize);
}

static void DrawBackground(const Canvas *canvas)
{
    const float widthPixels = CanvasGetWidthPixels(canvas);
    const float heightPixels = CanvasGetHeightPixels(canvas);

    DrawRectangle(0, 0, (int)widthPixels, (int)heightPixels, canvas->backgroundColor);
}

void CanvasDraw(const Canvas *canvas)
{
    if (!canvas || !canvas->pixels) return;

    DrawBackground(canvas);

    for (int y = 0; y < canvas->height; ++y)
    {
        for (int x = 0; x < canvas->width; ++x)
        {
            Color color = CanvasGetPixel(canvas, x, y);
            if (color.a == 0) continue;

            Rectangle rect = {
                (float)(x * canvas->pixelSize),
                (float)(y * canvas->pixelSize),
                (float)canvas->pixelSize,
                (float)canvas->pixelSize
            };
            DrawRectangleRec(rect, color);
        }
    }
}

void CanvasDrawGrid(const Canvas *canvas, Color gridColor, float lineThickness)
{
    if (!canvas) return;

    const float widthPixels = CanvasGetWidthPixels(canvas);
    const float heightPixels = CanvasGetHeightPixels(canvas);

    for (int x = 0; x <= canvas->width; ++x)
    {
        const float xPos = (float)(x * canvas->pixelSize);
        DrawLineEx((Vector2){ xPos, 0.0f }, (Vector2){ xPos, heightPixels }, lineThickness, gridColor);
    }

    for (int y = 0; y <= canvas->height; ++y)
    {
        const float yPos = (float)(y * canvas->pixelSize);
        DrawLineEx((Vector2){ 0.0f, yPos }, (Vector2){ widthPixels, yPos }, lineThickness, gridColor);
    }
}
