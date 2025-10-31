#include "tools.h"

#include "canvas.h"

void ToolsInit(ToolState *tools)
{
    if (!tools) return;
    tools->activeTool = TOOL_BRUSH;
    tools->foregroundColor = BLACK;
    tools->backgroundColor = WHITE;
    tools->isDrawing = false;
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

void ToolsHandleInput(ToolState *tools)
{
    if (!tools) return;

    if (IsKeyPressed(KEY_B)) tools->activeTool = TOOL_BRUSH;
    if (IsKeyPressed(KEY_E)) tools->activeTool = TOOL_ERASER;

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

bool ToolsApplyToCanvas(ToolState *tools, const Camera2D *camera, Canvas *canvas)
{
    if (!tools || !camera || !canvas) return false;

    bool updated = false;
    Vector2 mousePosition = GetMousePosition();
    Vector2 worldPosition = GetScreenToWorld2D(mousePosition, *camera);
    int pixelX = 0;
    int pixelY = 0;

    tools->isDrawing = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        if (CanvasWorldToPixel(canvas, worldPosition, &pixelX, &pixelY))
        {
            Color sampled = CanvasGetPixel(canvas, pixelX, pixelY);
            if (sampled.a > 0)
            {
                tools->foregroundColor = sampled;
            }
        }
    }

    if (!tools->isDrawing) return updated;

    if (CanvasWorldToPixel(canvas, worldPosition, &pixelX, &pixelY))
    {
        if (tools->activeTool == TOOL_ERASER)
        {
            CanvasSetPixel(canvas, pixelX, pixelY, BLANK);
        }
        else
        {
            CanvasSetPixel(canvas, pixelX, pixelY, tools->foregroundColor);
        }
        updated = true;
    }

    return updated;
}

const char *ToolsGetToolName(ToolType type)
{
    switch (type)
    {
        case TOOL_BRUSH: return "Brush";
        case TOOL_ERASER: return "Eraser";
        default: return "Unknown";
    }
}
