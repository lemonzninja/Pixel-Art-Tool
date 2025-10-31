#ifndef TOOLS_H
#define TOOLS_H

#include "raylib.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum ToolType {
    TOOL_BRUSH = 0,
    TOOL_ERASER,
    TOOL_LINE,
    TOOL_RECTANGLE_OUTLINE,
    TOOL_RECTANGLE_FILLED,
    TOOL_CIRCLE_OUTLINE,
    TOOL_CIRCLE_FILLED,
    TOOL_BUCKET_FILL,
    TOOL_COUNT
} ToolType;

typedef struct ToolState {
    ToolType activeTool;
    Color foregroundColor;
    Color backgroundColor;
    bool isDrawing;
    bool hasDragStart;
    int dragStartX;
    int dragStartY;
    bool hasLastPixel;
    int lastPixelX;
    int lastPixelY;
    bool hasLastCursor;
    int lastCursorX;
    int lastCursorY;
} ToolState;

typedef struct ToolDescriptor {
    ToolType type;
    const char *name;
    KeyboardKey shortcut;
    const char *shortcutLabel;
} ToolDescriptor;

struct Canvas;

void ToolsInit(ToolState *tools);
void ToolsHandleInput(ToolState *tools);
bool ToolsApplyToCanvas(ToolState *tools, const Camera2D *camera, struct Canvas *canvas);
const char *ToolsGetToolName(ToolType type);
const char *ToolsGetToolShortcutLabel(ToolType type);
size_t ToolsGetDescriptors(const ToolDescriptor **outDescriptors);

#endif // TOOLS_H
