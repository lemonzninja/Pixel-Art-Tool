#include "ui.h"

#include "canvas.h"

static Rectangle GetPaletteCell(int index);
static Rectangle GetToolButton(const AppState *state, bool eraser);

void HandlePaletteUI(AppState *state, Vector2 mouse, bool *interactingWithUI)
{
    if (!state)
    {
        return;
    }

    for (int i = 0; i < state->paletteCount; i++)
    {
        Rectangle paletteRect = GetPaletteCell(i);
        if (CheckCollisionPointRec(mouse, paletteRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            state->activeColorIndex = i;
            if (interactingWithUI)
            {
                *interactingWithUI = true;
            }
        }
    }
}

void HandleToolUI(AppState *state, Vector2 mouse, bool *interactingWithUI)
{
    if (!state)
    {
        return;
    }

    Rectangle pencilButton = GetToolButton(state, false);
    Rectangle eraserButton = GetToolButton(state, true);

    if (CheckCollisionPointRec(mouse, pencilButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        state->activeTool = TOOL_PENCIL;
        if (interactingWithUI)
        {
            *interactingWithUI = true;
        }
    }

    if (CheckCollisionPointRec(mouse, eraserButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        state->activeTool = TOOL_ERASER;
        if (interactingWithUI)
        {
            *interactingWithUI = true;
        }
    }
}

void DrawUI(const AppState *state)
{
    if (!state)
    {
        return;
    }

    for (int i = 0; i < state->paletteCount; i++)
    {
        Rectangle paletteRect = GetPaletteCell(i);
        DrawRectangleRec(paletteRect, state->palette[i]);
        DrawRectangleLinesEx(paletteRect, 2.0f, BLACK);

        if (i == state->activeColorIndex)
        {
            DrawRectangleLinesEx((Rectangle){ paletteRect.x - 3, paletteRect.y - 3, paletteRect.width + 6, paletteRect.height + 6 }, 2.0f, GOLD);
        }
    }

    Rectangle pencilButton = GetToolButton(state, false);
    Rectangle eraserButton = GetToolButton(state, true);

    DrawRectangleRec(pencilButton, Fade(LIGHTGRAY, state->activeTool == TOOL_PENCIL ? 0.6f : 0.2f));
    DrawRectangleLinesEx(pencilButton, 2.0f, DARKGRAY);
    DrawText("Pencil [1]", (int)pencilButton.x + 8, (int)pencilButton.y + 8, 16, BLACK);

    DrawRectangleRec(eraserButton, Fade(LIGHTGRAY, state->activeTool == TOOL_ERASER ? 0.6f : 0.2f));
    DrawRectangleLinesEx(eraserButton, 2.0f, DARKGRAY);
    DrawText("Eraser [2]", (int)eraserButton.x + 8, (int)eraserButton.y + 8, 16, BLACK);

    DrawText("Left click to draw. Click palette to change color.", 40, SCREEN_HEIGHT - 60, 16, DARKGRAY);
}

static Rectangle GetPaletteCell(int index)
{
    const float paletteCellSize = 32.0f;
    const float paletteSpacing = 12.0f;
    Rectangle canvasRect = GetCanvasScreenRect();
    Vector2 paletteOrigin = {
        CANVAS_POSITION.x + canvasRect.width + 40.0f,
        CANVAS_POSITION.y
    };

    return (Rectangle){
        paletteOrigin.x,
        paletteOrigin.y + index * (paletteCellSize + paletteSpacing),
        paletteCellSize,
        paletteCellSize
    };
}

static Rectangle GetToolButton(const AppState *state, bool eraser)
{
    const float paletteCellSize = 32.0f;
    const float paletteSpacing = 12.0f;
    const float toolSpacing = 12.0f;
    Rectangle canvasRect = GetCanvasScreenRect();
    Vector2 paletteOrigin = {
        CANVAS_POSITION.x + canvasRect.width + 40.0f,
        CANVAS_POSITION.y
    };

    int paletteCount = state ? state->paletteCount : 0;
    float baseY = paletteOrigin.y + paletteCount * (paletteCellSize + paletteSpacing) + 40.0f;
    Rectangle button = {
        paletteOrigin.x,
        baseY,
        140.0f,
        32.0f
    };

    if (eraser)
    {
        button.y += button.height + toolSpacing;
    }

    return button;
}
