#include "ui.h"

#include "canvas.h"

void UIInit(UIState *ui)
{
    if (!ui) return;
    ui->gridVisible = true;
}

void UIHandleInput(UIState *ui)
{
    if (!ui) return;

    if (IsKeyPressed(KEY_G))
    {
        ui->gridVisible = !ui->gridVisible;
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

    const int panelWidth = 240;
    const int panelHeight = 160;

    DrawRectangle(12, 12, panelWidth, panelHeight, Fade(DARKGRAY, 0.85f));
    DrawRectangleLines(12, 12, panelWidth, panelHeight, Fade(BLACK, 0.6f));

    int textY = 24;
    DrawText("Pixel Art Tool", 24, textY, 20, RAYWHITE);

    textY += 32;
    DrawText(TextFormat("Tool: %s (B/E)", ToolsGetToolName(tools->activeTool)), 24, textY, 16, RAYWHITE);

    textY += 24;
    DrawText("Left click: draw", 24, textY, 14, LIGHTGRAY);
    textY += 18;
    DrawText("Right click: eyedropper", 24, textY, 14, LIGHTGRAY);
    textY += 18;
    DrawText("Middle drag: pan", 24, textY, 14, LIGHTGRAY);
    textY += 18;
    DrawText("Mouse wheel: zoom", 24, textY, 14, LIGHTGRAY);

    textY += 22;
    DrawText(TextFormat("Zoom: %.0f%%", zoom * 100.0f), 24, textY, 16, RAYWHITE);
    textY += 22;
    DrawText(TextFormat("Grid: %s (G)", ui->gridVisible ? "On" : "Off"), 24, textY, 16, RAYWHITE);

    Vector2 swatchPos = { 24.0f, (float)(12 + panelHeight - 40) };
    DrawText("FG", (int)swatchPos.x, (int)swatchPos.y - 18, 16, RAYWHITE);
    DrawColorSwatch((Vector2){ swatchPos.x, swatchPos.y }, tools->foregroundColor);
    DrawText("BG", (int)swatchPos.x + 80, (int)swatchPos.y - 18, 16, RAYWHITE);
    DrawColorSwatch((Vector2){ swatchPos.x + 80, swatchPos.y }, tools->backgroundColor);

    DrawFPS(24, 12 + panelHeight - 24);
}
