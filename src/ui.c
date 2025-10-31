#include "ui.h"

#include "canvas.h"

#include <stddef.h>

static const int PANEL_X = 12;
static const int PANEL_Y = 12;
static const int PANEL_WIDTH = 260;
static const int TOOL_BUTTON_HEIGHT = 26;
static const int TOOL_BUTTON_SPACING = 6;
static const float TOOL_BUTTON_START_OFFSET = 84.0f;

static Rectangle GetPanelRect(size_t toolCount)
{
    int baseHeight = 220;
    int toolHeight = (int)toolCount * (TOOL_BUTTON_HEIGHT + TOOL_BUTTON_SPACING);
    Rectangle rect = {
        (float)PANEL_X,
        (float)PANEL_Y,
        (float)PANEL_WIDTH,
        (float)(baseHeight + toolHeight)
    };
    return rect;
}

static Rectangle GetToolButtonRect(size_t index, size_t toolCount)
{
    Rectangle panel = GetPanelRect(toolCount);
    float x = panel.x + 12.0f;
    float y = panel.y + TOOL_BUTTON_START_OFFSET + (float)index * (TOOL_BUTTON_HEIGHT + TOOL_BUTTON_SPACING);
    return (Rectangle){ x, y, panel.width - 24.0f, (float)TOOL_BUTTON_HEIGHT };
}

void UIInit(UIState *ui)
{
    if (!ui) return;
    ui->gridVisible = true;
}

void UIHandleInput(UIState *ui, ToolState *tools)
{
    if (!ui) return;

    if (IsKeyPressed(KEY_G))
    {
        ui->gridVisible = !ui->gridVisible;
    }

    if (!tools) return;

    size_t descriptorCount = 0;
    const ToolDescriptor *descriptors = ToolsGetDescriptors(&descriptorCount);
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        for (size_t i = 0; i < descriptorCount; ++i)
        {
            Rectangle buttonRect = GetToolButtonRect(i, descriptorCount);
            if (CheckCollisionPointRec(mouse, buttonRect))
            {
                if (tools->activeTool != descriptors[i].type)
                {
                    tools->activeTool = descriptors[i].type;
                    tools->hasDragStart = false;
                    tools->hasLastCursor = false;
                    tools->hasLastPixel = false;
                }
                break;
            }
        }
    }
}

static void DrawColorSwatch(Vector2 position, Color color)
{
    DrawRectangleV(position, (Vector2){ 24, 24 }, color);
    DrawRectangleLinesEx((Rectangle){ position.x, position.y, 24, 24 }, 1.0f, Fade(BLACK, 0.8f));
}

void UIDraw(const UIState *ui, const ToolState *tools, float zoom)
{
    if (!ui || !tools) return;

    size_t descriptorCount = 0;
    const ToolDescriptor *descriptors = ToolsGetDescriptors(&descriptorCount);
    Rectangle panel = GetPanelRect(descriptorCount);

    DrawRectangle((int)panel.x, (int)panel.y, (int)panel.width, (int)panel.height, Fade(DARKGRAY, 0.85f));
    DrawRectangleLines((int)panel.x, (int)panel.y, (int)panel.width, (int)panel.height, Fade(BLACK, 0.6f));

    int textX = PANEL_X + 12;
    int textY = PANEL_Y + 12;
    DrawText("Pixel Art Tool", textX, textY, 20, RAYWHITE);

    textY += 28;
    DrawText(TextFormat("Active: %s (%s)", ToolsGetToolName(tools->activeTool), ToolsGetToolShortcutLabel(tools->activeTool)), textX, textY, 16, RAYWHITE);

    textY += 28;
    DrawText("Tools", textX, textY, 18, RAYWHITE);

    Color buttonOutline = Fade(BLACK, 0.5f);
    for (size_t i = 0; i < descriptorCount; ++i)
    {
        Rectangle buttonRect = GetToolButtonRect(i, descriptorCount);
        bool isActive = (tools->activeTool == descriptors[i].type);
        bool isHovered = CheckCollisionPointRec(GetMousePosition(), buttonRect);

        Color fillColor = isActive ? Fade(SKYBLUE, 0.6f) : Fade(LIGHTGRAY, 0.25f);
        if (!isActive && isHovered) fillColor = Fade(LIGHTGRAY, 0.4f);

        DrawRectangleRec(buttonRect, fillColor);
        DrawRectangleLinesEx(buttonRect, 1.0f, buttonOutline);

        DrawText(TextFormat("%s (%s)", descriptors[i].name, descriptors[i].shortcutLabel), (int)buttonRect.x + 8, (int)buttonRect.y + 5, 16, RAYWHITE);
    }

    int infoY = (int)(panel.y + TOOL_BUTTON_START_OFFSET + (float)descriptorCount * (TOOL_BUTTON_HEIGHT + TOOL_BUTTON_SPACING) + 16.0f);
    DrawText("Left click: draw/apply", textX, infoY, 14, LIGHTGRAY);
    infoY += 16;
    DrawText("Right click: eyedropper", textX, infoY, 14, LIGHTGRAY);
    infoY += 16;
    DrawText("Shapes: click and drag", textX, infoY, 14, LIGHTGRAY);
    infoY += 16;
    DrawText("Bucket fill: click area", textX, infoY, 14, LIGHTGRAY);
    infoY += 20;
    DrawText(TextFormat("Zoom: %.0f%%", zoom * 100.0f), textX, infoY, 16, RAYWHITE);
    infoY += 20;
    DrawText(TextFormat("Grid: %s (G)", ui->gridVisible ? "On" : "Off"), textX, infoY, 16, RAYWHITE);

    Vector2 swatchPos = { (float)textX, panel.y + panel.height - 48.0f };
    DrawText("FG", (int)swatchPos.x, (int)swatchPos.y - 18, 16, RAYWHITE);
    DrawColorSwatch(swatchPos, tools->foregroundColor);
    DrawText("BG", (int)swatchPos.x + 80, (int)swatchPos.y - 18, 16, RAYWHITE);
    DrawColorSwatch((Vector2){ swatchPos.x + 80, swatchPos.y }, tools->backgroundColor);

    DrawFPS(PANEL_X + 12, (int)(panel.y + panel.height - 24));
}
