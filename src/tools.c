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

    switch (state->activeTool)
    {
        case TOOL_PENCIL:
            ImageDrawPixel(&state->canvasImage, canvasX, canvasY, state->palette[state->activeColorIndex]);
            return true;
        case TOOL_ERASER:
            ImageDrawPixel(&state->canvasImage, canvasX, canvasY, BLANK);
            return true;
        default:
            break;
    }

    return false;
}
