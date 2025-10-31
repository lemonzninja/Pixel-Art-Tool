/**************************************************************************************
*                   - Core Canvas System -                                                    
*  Create a 2D array or dynamic structure to store pixel data (colors)                                                                                 
*  Render the pixel grid using DrawRectangle() or a texture-based approach                                                                     
*  Add grid overlay that can be toggled on/off             
**************************************************************************************/

#include "canvas.h"
#include "tools.h"

static bool CanvasCoordsFromMouse(Vector2 mouse, int *x, int *y);

void InitCanvas(AppState *state)
{
    if (!state)
    {
        return;
    }

    state->canvasImage = GenImageColor(CANVAS_PIXEL_WIDTH, CANVAS_PIXEL_HEIGHT, BLANK);
    state->canvasTexture = LoadTextureFromImage(state->canvasImage);
    SetTextureFilter(state->canvasTexture, TEXTURE_FILTER_POINT);
}

void ShutdownCanvas(AppState *state)
{
    if (!state)
    {
        return;
    }

    UnloadTexture(state->canvasTexture);
    UnloadImage(state->canvasImage);
}

void HandleCanvasInput(AppState *state, Vector2 mouse, bool *pixelModified, bool interactingWithUI)
{
    if (!state || !pixelModified)
    {
        return;
    }

    *pixelModified = false;

    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) || interactingWithUI)
    {
        return;
    }

    int canvasX = 0;
    int canvasY = 0;
    if (CanvasCoordsFromMouse(mouse, &canvasX, &canvasY))
    {
        if (ApplyTool(state, canvasX, canvasY))
        {
            *pixelModified = true;
            UpdateTexture(state->canvasTexture, state->canvasImage.data);
        }
    }
}

void DrawCanvas(const AppState *state, Vector2 mouse)
{
    if (!state)
    {
        return;
    }

    Rectangle canvasRect = GetCanvasScreenRect();
    DrawTextureEx(state->canvasTexture, (Vector2){ canvasRect.x, canvasRect.y }, 0.0f, (float)CANVAS_ZOOM, WHITE);
    DrawRectangleLinesEx(canvasRect, 2.0f, DARKGRAY);

    int canvasX = 0;
    int canvasY = 0;
    if (CanvasCoordsFromMouse(mouse, &canvasX, &canvasY))
    {
        Rectangle hoverRect = {
            CANVAS_POSITION.x + canvasX * CANVAS_ZOOM,
            CANVAS_POSITION.y + canvasY * CANVAS_ZOOM,
            (float)CANVAS_ZOOM,
            (float)CANVAS_ZOOM
        };
        DrawRectangleLinesEx(hoverRect, 1.0f, Fade(BLACK, 0.5f));
    }
}

Rectangle GetCanvasScreenRect(void)
{
    return (Rectangle){
        CANVAS_POSITION.x,
        CANVAS_POSITION.y,
        (float)CANVAS_PIXEL_WIDTH * CANVAS_ZOOM,
        (float)CANVAS_PIXEL_HEIGHT * CANVAS_ZOOM
    };
}

static bool CanvasCoordsFromMouse(Vector2 mouse, int *x, int *y)
{
    Rectangle canvasRect = GetCanvasScreenRect();
    if (!CheckCollisionPointRec(mouse, canvasRect))
    {
        return false;
    }

    int localX = (int)((mouse.x - CANVAS_POSITION.x) / CANVAS_ZOOM);
    int localY = (int)((mouse.y - CANVAS_POSITION.y) / CANVAS_ZOOM);

    if (localX < 0 || localY < 0 || localX >= CANVAS_PIXEL_WIDTH || localY >= CANVAS_PIXEL_HEIGHT)
    {
        return false;
    }

    if (x)
    {
        *x = localX;
    }
    if (y)
    {
        *y = localY;
    }

    return true;
}
