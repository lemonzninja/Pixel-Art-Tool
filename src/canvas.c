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

static int MinInt(int a, int b)
{
    return (a < b) ? a : b;
}

static int MaxInt(int a, int b)
{
    return (a > b) ? a : b;
}

void CanvasDrawLine(Canvas *canvas, int x0, int y0, int x1, int y1, Color color)
{
    if (!canvas) return;

    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        CanvasSetPixel(canvas, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void CanvasDrawRectangle(Canvas *canvas, int x0, int y0, int x1, int y1, Color color)
{
    if (!canvas) return;

    int left = MinInt(x0, x1);
    int right = MaxInt(x0, x1);
    int top = MinInt(y0, y1);
    int bottom = MaxInt(y0, y1);

    for (int x = left; x <= right; ++x)
    {
        CanvasSetPixel(canvas, x, top, color);
        CanvasSetPixel(canvas, x, bottom, color);
    }

    for (int y = top; y <= bottom; ++y)
    {
        CanvasSetPixel(canvas, left, y, color);
        CanvasSetPixel(canvas, right, y, color);
    }
}

void CanvasFillRectangle(Canvas *canvas, int x0, int y0, int x1, int y1, Color color)
{
    if (!canvas) return;

    int left = MinInt(x0, x1);
    int right = MaxInt(x0, x1);
    int top = MinInt(y0, y1);
    int bottom = MaxInt(y0, y1);

    for (int y = top; y <= bottom; ++y)
    {
        for (int x = left; x <= right; ++x)
        {
            CanvasSetPixel(canvas, x, y, color);
        }
    }
}

static void DrawCircleSymmetricPoints(Canvas *canvas, int centerX, int centerY, int x, int y, Color color)
{
    CanvasSetPixel(canvas, centerX + x, centerY + y, color);
    CanvasSetPixel(canvas, centerX - x, centerY + y, color);
    CanvasSetPixel(canvas, centerX + x, centerY - y, color);
    CanvasSetPixel(canvas, centerX - x, centerY - y, color);
    CanvasSetPixel(canvas, centerX + y, centerY + x, color);
    CanvasSetPixel(canvas, centerX - y, centerY + x, color);
    CanvasSetPixel(canvas, centerX + y, centerY - x, color);
    CanvasSetPixel(canvas, centerX - y, centerY - x, color);
}

void CanvasDrawCircle(Canvas *canvas, int centerX, int centerY, int radius, Color color)
{
    if (!canvas) return;
    if (radius < 0) radius = 0;

    int x = radius;
    int y = 0;
    int err = 1 - radius;

    while (x >= y)
    {
        DrawCircleSymmetricPoints(canvas, centerX, centerY, x, y, color);
        y++;
        if (err < 0)
        {
            err += 2 * y + 1;
        }
        else
        {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

static void DrawHorizontalSpan(Canvas *canvas, int x0, int x1, int y, Color color)
{
    if (!canvas) return;

    int left = MinInt(x0, x1);
    int right = MaxInt(x0, x1);
    for (int x = left; x <= right; ++x)
    {
        CanvasSetPixel(canvas, x, y, color);
    }
}

void CanvasFillCircle(Canvas *canvas, int centerX, int centerY, int radius, Color color)
{
    if (!canvas) return;
    if (radius < 0) radius = 0;

    int x = radius;
    int y = 0;
    int err = 1 - radius;

    while (x >= y)
    {
        DrawHorizontalSpan(canvas, centerX - x, centerX + x, centerY + y, color);
        DrawHorizontalSpan(canvas, centerX - x, centerX + x, centerY - y, color);
        DrawHorizontalSpan(canvas, centerX - y, centerX + y, centerY + x, color);
        DrawHorizontalSpan(canvas, centerX - y, centerX + y, centerY - x, color);

        y++;
        if (err < 0)
        {
            err += 2 * y + 1;
        }
        else
        {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

typedef struct FillPoint
{
    int x;
    int y;
} FillPoint;

static bool ColorsEqual(Color a, Color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

bool CanvasFloodFill(Canvas *canvas, int startX, int startY, Color targetColor, Color replacementColor)
{
    if (!canvas || !canvas->pixels) return false;
    if (!CanvasContains(canvas, startX, startY)) return false;
    if (ColorsEqual(targetColor, replacementColor)) return false;

    size_t capacity = (size_t)canvas->width * (size_t)canvas->height;
    if (capacity == 0) return false;

    FillPoint *stack = (FillPoint *)MemAlloc(capacity * sizeof(FillPoint));
    if (!stack) return false;

    size_t top = 0;
    stack[top++] = (FillPoint){ startX, startY };
    bool changed = false;

    while (top > 0)
    {
        FillPoint point = stack[--top];

        if (!CanvasContains(canvas, point.x, point.y)) continue;

        Color current = CanvasGetPixel(canvas, point.x, point.y);
        if (!ColorsEqual(current, targetColor)) continue;

        CanvasSetPixel(canvas, point.x, point.y, replacementColor);
        changed = true;

        FillPoint neighbors[4] = {
            { point.x + 1, point.y },
            { point.x - 1, point.y },
            { point.x, point.y + 1 },
            { point.x, point.y - 1 }
        };

        for (size_t i = 0; i < 4; ++i)
        {
            FillPoint neighbor = neighbors[i];
            if (!CanvasContains(canvas, neighbor.x, neighbor.y)) continue;

            if (top >= capacity)
            {
                size_t newCapacity = capacity * 2;
                if (newCapacity < capacity + 4) newCapacity = capacity + 4;
                FillPoint *resized = (FillPoint *)MemRealloc(stack, newCapacity * sizeof(FillPoint));
                if (!resized)
                {
                    MemFree(stack);
                    return changed;
                }
                stack = resized;
                capacity = newCapacity;
            }

            stack[top++] = neighbor;
        }
    }

    MemFree(stack);
    return changed;
}
