#ifndef CANVAS_H
#define CANVAS_H

#include "raylib.h"
#include <stdbool.h>

typedef struct Canvas {
    int width;
    int height;
    int pixelSize;
    Color *pixels;
    Color backgroundColor;
} Canvas;

bool CanvasInit(Canvas *canvas, int width, int height, int pixelSize, Color initialColor, Color backgroundColor);
void CanvasUnload(Canvas *canvas);
void CanvasClear(Canvas *canvas, Color color);
void CanvasSetPixel(Canvas *canvas, int x, int y, Color color);
Color CanvasGetPixel(const Canvas *canvas, int x, int y);
bool CanvasContains(const Canvas *canvas, int x, int y);
bool CanvasWorldToPixel(const Canvas *canvas, Vector2 worldPosition, int *outX, int *outY);
void CanvasDraw(const Canvas *canvas);
void CanvasDrawGrid(const Canvas *canvas, Color gridColor, float lineThickness);
float CanvasGetWidthPixels(const Canvas *canvas);
float CanvasGetHeightPixels(const Canvas *canvas);
void CanvasDrawLine(Canvas *canvas, int x0, int y0, int x1, int y1, Color color);
void CanvasDrawRectangle(Canvas *canvas, int x0, int y0, int x1, int y1, Color color);
void CanvasFillRectangle(Canvas *canvas, int x0, int y0, int x1, int y1, Color color);
void CanvasDrawCircle(Canvas *canvas, int centerX, int centerY, int radius, Color color);
void CanvasFillCircle(Canvas *canvas, int centerX, int centerY, int radius, Color color);
bool CanvasFloodFill(Canvas *canvas, int startX, int startY, Color targetColor, Color replacementColor);

#endif // CANVAS_H
