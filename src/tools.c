#include "tools.h"

#include "raylib.h"

void HandleToolShortcuts(AppState *state)
{
    if (!state)
    {
        return;
    }

    if (IsKeyPressed(KEY_ONE))
    {
        state->activeTool = TOOL_PENCIL;
    }
    if (IsKeyPressed(KEY_TWO))
    {
        state->activeTool = TOOL_ERASER;
    }
}

bool ApplyTool(AppState *state, int canvasX, int canvasY)
{
    if (!state)
    {
        return false;
    }

    Canvas *canvas = &state->canvas;
    if (!canvas->pixels || canvasX < 0 || canvasY < 0 || canvasX >= canvas->width || canvasY >= canvas->height)
    {
        return false;
    }

    int index = canvasY * canvas->width + canvasX;

    switch (state->activeTool)
    {
        case TOOL_PENCIL:
            canvas->pixels[index] = state->palette[state->activeColorIndex];
            return true;
        case TOOL_ERASER:
            canvas->pixels[index] = BLANK;
            return true;
        default:
            break;
    }

    return false;
}
