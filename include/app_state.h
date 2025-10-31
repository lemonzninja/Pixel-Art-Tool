#ifndef APP_STATE_H
#define APP_STATE_H

#include "raylib.h"

typedef enum ToolType {
    TOOL_PENCIL = 0,
    TOOL_ERASER = 1
} ToolType;

typedef struct AppState {
    Image canvasImage;
    Texture2D canvasTexture;
    ToolType activeTool;
    int activeColorIndex;
    Color palette[6];
    int paletteCount;
} AppState;

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 450;
static const int CANVAS_PIXEL_WIDTH = 64;
static const int CANVAS_PIXEL_HEIGHT = 64;
static const int CANVAS_ZOOM = 8;
static const Vector2 CANVAS_POSITION = { 40.0f, 40.0f };

#endif // APP_STATE_H
