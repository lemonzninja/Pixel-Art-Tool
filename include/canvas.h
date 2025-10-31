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

#endif // CANVAS_H
