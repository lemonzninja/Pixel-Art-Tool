#ifndef TOOLS_H
#define TOOLS_H

#include "raylib.h"
#include <stdbool.h>

typedef enum ToolType {
    TOOL_BRUSH = 0,
    TOOL_ERASER
} ToolType;

typedef struct ToolState {
    ToolType activeTool;
    Color foregroundColor;
    Color backgroundColor;
    bool isDrawing;
} ToolState;

struct Canvas;

void ToolsInit(ToolState *tools);
void ToolsHandleInput(ToolState *tools);
bool ToolsApplyToCanvas(ToolState *tools, const Camera2D *camera, struct Canvas *canvas);
const char *ToolsGetToolName(ToolType type);

#endif // TOOLS_H
