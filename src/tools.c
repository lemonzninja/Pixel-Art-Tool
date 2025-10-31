#include "tools.h"

#include "canvas.h"

#include <math.h>

static const ToolDescriptor TOOL_DESCRIPTORS[TOOL_COUNT] = {
    { TOOL_BRUSH, "Brush", KEY_B, "B" },
    { TOOL_ERASER, "Eraser", KEY_E, "E" },
    { TOOL_LINE, "Line", KEY_L, "L" },
    { TOOL_RECTANGLE_OUTLINE, "Rectangle (Outline)", KEY_R, "R" },
    { TOOL_RECTANGLE_FILLED, "Rectangle (Fill)", KEY_T, "T" },
    { TOOL_CIRCLE_OUTLINE, "Circle (Outline)", KEY_O, "O" },
    { TOOL_CIRCLE_FILLED, "Circle (Fill)", KEY_U, "U" },
    { TOOL_BUCKET_FILL, "Bucket Fill", KEY_F, "F" }
};

static const ToolDescriptor *FindDescriptor(ToolType type)
{
    for (size_t i = 0; i < TOOL_COUNT; ++i)
    {
        if (TOOL_DESCRIPTORS[i].type == type) return &TOOL_DESCRIPTORS[i];
    }
    return NULL;
}

void ToolsInit(ToolState *tools)
{
    if (!tools) return;
    tools->activeTool = TOOL_BRUSH;
    tools->foregroundColor = BLACK;
    tools->backgroundColor = WHITE;
    tools->isDrawing = false;
    tools->hasDragStart = false;
    tools->dragStartX = 0;
    tools->dragStartY = 0;
    tools->hasLastPixel = false;
    tools->lastPixelX = 0;
    tools->lastPixelY = 0;
    tools->hasLastCursor = false;
    tools->lastCursorX = 0;
    tools->lastCursorY = 0;
}

static Color GetPaletteColor(int index)
{
    switch (index)
    {
        case 0: return BLACK;
        case 1: return RED;
        case 2: return GREEN;
        case 3: return BLUE;
        case 4: return PURPLE;
        case 5: return WHITE;
        default: return BLACK;
    }
}

static void ResetTransientState(ToolState *tools)
{
    tools->hasDragStart = false;
    tools->hasLastPixel = false;
    tools->hasLastCursor = false;
}

void ToolsHandleInput(ToolState *tools)
{
    if (!tools) return;

    ToolType previousTool = tools->activeTool;
    for (size_t i = 0; i < TOOL_COUNT; ++i)
    {
        if (IsKeyPressed(TOOL_DESCRIPTORS[i].shortcut))
        {
            tools->activeTool = TOOL_DESCRIPTORS[i].type;
        }
    }

    if (tools->activeTool != previousTool)
    {
        ResetTransientState(tools);
    }

    if (IsKeyPressed(KEY_ONE)) tools->foregroundColor = GetPaletteColor(0);
    if (IsKeyPressed(KEY_TWO)) tools->foregroundColor = GetPaletteColor(1);
    if (IsKeyPressed(KEY_THREE)) tools->foregroundColor = GetPaletteColor(2);
    if (IsKeyPressed(KEY_FOUR)) tools->foregroundColor = GetPaletteColor(3);
    if (IsKeyPressed(KEY_FIVE)) tools->foregroundColor = GetPaletteColor(4);
    if (IsKeyPressed(KEY_SIX)) tools->foregroundColor = GetPaletteColor(5);

    if (IsKeyPressed(KEY_X))
    {
        Color temp = tools->foregroundColor;
        tools->foregroundColor = tools->backgroundColor;
        tools->backgroundColor = temp;
    }
}

static void UpdateDragState(ToolState *tools, bool cursorValid, int pixelX, int pixelY)
{
    if (!tools) return;
    if (cursorValid)
    {
        tools->lastCursorX = pixelX;
        tools->lastCursorY = pixelY;
        tools->hasLastCursor = true;
    }
    else if (!tools->isDrawing)
    {
        tools->hasLastCursor = false;
    }
}

static void HandleBucketFill(ToolState *tools, Canvas *canvas, int pixelX, int pixelY, bool cursorValid, bool *updated)
{
    if (!tools || !canvas || !updated) return;
    if (!cursorValid) return;

    Color target = CanvasGetPixel(canvas, pixelX, pixelY);
    if (CanvasFloodFill(canvas, pixelX, pixelY, target, tools->foregroundColor))
    {
        *updated = true;
    }
}

static void DrawBrushStroke(ToolState *tools, Canvas *canvas, int pixelX, int pixelY, bool cursorValid, bool *updated)
{
    if (!tools || !canvas || !updated) return;
    if (!cursorValid) return;

    Color drawColor = (tools->activeTool == TOOL_BRUSH) ? tools->foregroundColor : BLANK;
    if (tools->hasLastPixel)
    {
        CanvasDrawLine(canvas, tools->lastPixelX, tools->lastPixelY, pixelX, pixelY, drawColor);
    }
    else
    {
        CanvasSetPixel(canvas, pixelX, pixelY, drawColor);
    }

    tools->lastPixelX = pixelX;
    tools->lastPixelY = pixelY;
    tools->hasLastPixel = true;
    *updated = true;
}

static void ApplyShape(ToolState *tools, Canvas *canvas, int endX, int endY, bool *updated)
{
    if (!tools || !canvas || !updated) return;
    if (!tools->hasDragStart) return;

    int startX = tools->dragStartX;
    int startY = tools->dragStartY;

    switch (tools->activeTool)
    {
        case TOOL_LINE:
            CanvasDrawLine(canvas, startX, startY, endX, endY, tools->foregroundColor);
            *updated = true;
            break;
        case TOOL_RECTANGLE_OUTLINE:
            CanvasDrawRectangle(canvas, startX, startY, endX, endY, tools->foregroundColor);
            *updated = true;
            break;
        case TOOL_RECTANGLE_FILLED:
            CanvasFillRectangle(canvas, startX, startY, endX, endY, tools->foregroundColor);
            *updated = true;
            break;
        case TOOL_CIRCLE_OUTLINE:
        case TOOL_CIRCLE_FILLED:
        {
            int left = startX < endX ? startX : endX;
            int right = startX > endX ? startX : endX;
            int top = startY < endY ? startY : endY;
            int bottom = startY > endY ? startY : endY;

            int width = right - left;
            int height = bottom - top;
            float centerXf = left + width * 0.5f;
            float centerYf = top + height * 0.5f;
            int centerX = (int)lroundf(centerXf);
            int centerY = (int)lroundf(centerYf);
            int diameter = (width > height) ? width : height;
            int radius = (int)lroundf((float)diameter * 0.5f);
            if (radius < 0) radius = 0;

            if (tools->activeTool == TOOL_CIRCLE_OUTLINE)
            {
                CanvasDrawCircle(canvas, centerX, centerY, radius, tools->foregroundColor);
            }
            else
            {
                CanvasFillCircle(canvas, centerX, centerY, radius, tools->foregroundColor);
            }
            *updated = true;
            break;
        }
        default:
            break;
    }
}

bool ToolsApplyToCanvas(ToolState *tools, const Camera2D *camera, Canvas *canvas)
{
    if (!tools || !camera || !canvas) return false;

    bool updated = false;
    Vector2 mousePosition = GetMousePosition();
    Vector2 worldPosition = GetScreenToWorld2D(mousePosition, *camera);
    int pixelX = 0;
    int pixelY = 0;
    bool cursorValid = CanvasWorldToPixel(canvas, worldPosition, &pixelX, &pixelY);

    bool leftPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool leftDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool leftReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    tools->isDrawing = leftDown;

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        if (cursorValid)
        {
            Color sampled = CanvasGetPixel(canvas, pixelX, pixelY);
            if (sampled.a > 0)
            {
                tools->foregroundColor = sampled;
            }
        }
    }

    if (leftPressed && cursorValid)
    {
        tools->dragStartX = pixelX;
        tools->dragStartY = pixelY;
        tools->hasDragStart = true;
        tools->hasLastPixel = false;
        tools->lastCursorX = pixelX;
        tools->lastCursorY = pixelY;
        tools->hasLastCursor = true;
    }

    if (leftPressed && tools->activeTool == TOOL_BUCKET_FILL)
    {
        HandleBucketFill(tools, canvas, pixelX, pixelY, cursorValid, &updated);
    }

    if (leftDown)
    {
        UpdateDragState(tools, cursorValid, pixelX, pixelY);

        if (tools->activeTool == TOOL_BRUSH || tools->activeTool == TOOL_ERASER)
        {
            DrawBrushStroke(tools, canvas, pixelX, pixelY, cursorValid, &updated);
        }
    }
    else
    {
        tools->hasLastPixel = false;
    }

    if (leftReleased)
    {
        int endX = cursorValid ? pixelX : tools->lastCursorX;
        int endY = cursorValid ? pixelY : tools->lastCursorY;
        bool hasEnd = cursorValid || tools->hasLastCursor;

        if (hasEnd)
        {
            ApplyShape(tools, canvas, endX, endY, &updated);
        }

        tools->hasDragStart = false;
        tools->hasLastCursor = false;
        tools->hasLastPixel = false;
    }

    return updated;
}

const char *ToolsGetToolName(ToolType type)
{
    const ToolDescriptor *descriptor = FindDescriptor(type);
    return descriptor ? descriptor->name : "Unknown";
}

const char *ToolsGetToolShortcutLabel(ToolType type)
{
    const ToolDescriptor *descriptor = FindDescriptor(type);
    return descriptor ? descriptor->shortcutLabel : "";
}

size_t ToolsGetDescriptors(const ToolDescriptor **outDescriptors)
{
    if (outDescriptors)
    {
        *outDescriptors = TOOL_DESCRIPTORS;
    }
    return TOOL_COUNT;
}
